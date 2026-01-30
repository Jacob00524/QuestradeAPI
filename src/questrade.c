#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "questrade.h"

/*
    callback and consumer key can be NULL if not using login from code
*/
int questrade_init(char *consumer_key, char *callback, void(*token_refresh_callback)(questrade_tokens))
{
    questrade_settings settings = { 0 };

    if (consumer_key)
    {
        settings.consumer_key = malloc(strlen(consumer_key) + 1);
        strcpy(settings.consumer_key, consumer_key);
    }

    if (callback)
    {
        settings.callback_url = malloc(strlen(callback) + 1);
        strcpy(settings.callback_url, callback);
    }

    if (token_refresh_callback)
        settings.token_refresh_callback = token_refresh_callback;
    
    questrade_set_settings(settings);
    return 1;
}

void questrade_cleanup()
{
    questrade_login_cleanup();
    questrade_globals_cleanup(); /* should always be called last */
}