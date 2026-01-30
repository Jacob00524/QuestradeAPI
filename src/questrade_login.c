#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <string.h>
#include <stdatomic.h>
#include <errno.h>

#include "curlwrapper.h"
#include "questrade.h"

static volatile atomic_int request_shutdown = 0;
static pthread_mutex_t questrade_thread_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t  token_cv  = PTHREAD_COND_INITIALIZER;
static int thread_running = 0;

/*
    A user would navigate to the following url with their questrade client_id (consumer key) and their redirect uri
    Both must be registered correctly in the app hub.

    https://login.questrade.com/oauth2/authorize?client_id=<client_id> &response_type=code&redirect_uri=https://www.example.com

    Once login is successfull, questrade will redirect the user to url specified in the query along with a code passed in the url
    Use this code to for this function.
*/
int questrade_login_from_code(char *code)
{
    long response_status;
    char request_url[512], *returned_tokens;
    questrade_settings settings = questrade_get_settings();
    questrade_tokens tokens = { 0 };

    if (!code || !code[0] || !settings.consumer_key || !settings.callback_url)
        return 0;

    snprintf(request_url, sizeof(request_url), "client_id=%s&code=%s&grant_type=authorization_code&redirect_uri=%s", settings.consumer_key, code, settings.callback_url);
    returned_tokens = http_post("https://login.questrade.com/oauth2/token", NULL, request_url, 0, &response_status);
    tokens.time_refreshed = time(NULL);

    if (response_status != 200 || !returned_tokens)
    {
        if (returned_tokens)
            free(returned_tokens);
        return 0;
    }

    if (!questrade_parse_tokens(returned_tokens, &tokens))
    {
        free(returned_tokens);
        return 0;
    }

    questrade_set_tokens(tokens);
    free(returned_tokens);
    if (settings.token_refresh_callback)
        settings.token_refresh_callback(tokens);
    return 1;
}

/*
    get new refresh tokens using current non-expired refresh tokens
*/
int questrade_login_from_refresh(char *token)
{
    long response_status;
    char request_url[512], *returned_tokens;
    questrade_tokens tokens = { 0 };
    questrade_settings settings = questrade_get_settings();

    if (!token || !token[0])
        return 0;

    snprintf(request_url, sizeof(request_url), "https://login.questrade.com/oauth2/token?grant_type=refresh_token&refresh_token=%s", token);
    returned_tokens = http_get(request_url, NULL, &response_status);
    tokens.time_refreshed = time(NULL);

    if (response_status != 200 || !returned_tokens)
    {
        if (returned_tokens)
            free(returned_tokens);
        return 0;
    }

    if (!questrade_parse_tokens(returned_tokens, &tokens))
    {
        free(returned_tokens);
        return 0;
    }

    questrade_set_tokens(tokens);
    free(returned_tokens);
    if (settings.token_refresh_callback)
        settings.token_refresh_callback(tokens);
    return 1;
}

static void wait_or_shutdown_seconds(int seconds)
{
    if (seconds < 0) seconds = 0;

    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    ts.tv_sec += seconds;

    pthread_mutex_lock(&questrade_thread_mutex);
    while (!atomic_load(&request_shutdown))
    {
        int rc = pthread_cond_timedwait(&token_cv, &questrade_thread_mutex, &ts);
        if (rc == ETIMEDOUT) break;
    }
    pthread_mutex_unlock(&questrade_thread_mutex);
}

/*
    Keeps authorization up to-date
*/
static void* token_thread(void* param)
{
    questrade_tokens current_tokens = questrade_get_tokens();
    (void)param;

    while (current_tokens.expires_in == 0 && request_shutdown == 0)
    {
        wait_or_shutdown_seconds(1);
        current_tokens = questrade_get_tokens(); /* sleep until first login */
    }
    while (request_shutdown == 0)
    {
        time_t time_now = time(NULL);
        time_t time_expired = current_tokens.time_refreshed + (time_t)current_tokens.expires_in;
        time_t time_remaining = time_expired - time_now;
        time_t sleep_for = time_remaining - 5;
        if (sleep_for < 1) sleep_for = 1;
        wait_or_shutdown_seconds((unsigned)sleep_for);
        current_tokens = questrade_get_tokens();

        time_now = time(NULL);
        time_expired = current_tokens.time_refreshed + (time_t)current_tokens.expires_in;
        time_remaining = time_expired - time_now;
        if (time_remaining > 10) continue;

        if (!questrade_login_from_refresh(current_tokens.refresh_token))
        {
            printf("Thread refresh login error.\n");
            wait_or_shutdown_seconds(5);
            current_tokens = questrade_get_tokens();
            continue;
        }
        current_tokens = questrade_get_tokens();
    }
    return NULL;
}

static int create_token_thread()
{
    pthread_t thread;

    pthread_mutex_lock(&questrade_thread_mutex);
    if (thread_running)
    {
        pthread_mutex_unlock(&questrade_thread_mutex);
        return 0;
    }
    thread_running = 1;
    pthread_mutex_unlock(&questrade_thread_mutex);

    pthread_create(&thread, NULL, token_thread, NULL);
    return 1;
}

int questrade_start_login_routine(questrade_tokens current_tokens)
{
    time_t time_now, time_expires;

    time_expires = current_tokens.time_refreshed + current_tokens.expires_in - 5; /* 5 is a buffer to allow refresh */
    time_now = time(NULL);

    if (time_now < time_expires)
    {
        /* not expired, therefore it can be refreshed */
        if (questrade_login_from_refresh(current_tokens.refresh_token))
        {
            create_token_thread();
            return 1;
        }else
            return 0;
    }
    return 0;
}

void questrade_stop_login_routine()
{
    if (thread_running)
    {
        request_shutdown = 1;

        pthread_mutex_lock(&questrade_thread_mutex);
        pthread_cond_broadcast(&token_cv);
        pthread_mutex_unlock(&questrade_thread_mutex);
    }
}

int questrade_is_tokens_expired(questrade_tokens tokens)
{
    time_t time_now, time_expired;

    time_expired = tokens.time_refreshed + tokens.expires_in;
    time_now = time(NULL);
    if (time_now >= time_expired)
        return 0;
    return 1;
}

void questrade_login_cleanup()
{
    questrade_stop_login_routine();
    pthread_mutex_destroy(&questrade_thread_mutex);
    pthread_cond_destroy(&token_cv);
}