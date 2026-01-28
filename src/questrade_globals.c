#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include "questrade.h"

static questrade_settings global_questrade_settings = { 0 };
static pthread_mutex_t questrade_settings_mutex = PTHREAD_MUTEX_INITIALIZER;

static questrade_tokens global_questrade_tokens = { 0 };
static pthread_mutex_t questrade_tokens_mutex = PTHREAD_MUTEX_INITIALIZER;

static questrade_account* global_accounts = NULL;
static int global_accounts_count = 0;
static pthread_mutex_t questrade_accounts_mutex = PTHREAD_MUTEX_INITIALIZER;

static questrade_balance* global_balances = NULL;
static int global_balances_count = 0;
static pthread_mutex_t questrade_balances_mutex = PTHREAD_MUTEX_INITIALIZER;

static questrade_position* global_positions = NULL;
static int global_positions_count = 0;
static pthread_mutex_t questrade_positions_mutex = PTHREAD_MUTEX_INITIALIZER;

static questrade_quote* global_quotes = NULL;
static int global_quotes_count = 0;
static pthread_mutex_t questrade_quotes_mutex = PTHREAD_MUTEX_INITIALIZER;

/*
    Settings
*/
questrade_settings questrade_get_settings()
{
    questrade_settings copy;

    pthread_mutex_lock(&questrade_settings_mutex);
    copy = global_questrade_settings;
    pthread_mutex_unlock(&questrade_settings_mutex);

    return copy;
}

void questrade_set_settings(questrade_settings settings)
{
    pthread_mutex_lock(&questrade_settings_mutex);
    global_questrade_settings = settings;
    pthread_mutex_unlock(&questrade_settings_mutex);
}

/*
    Tokens
*/
questrade_tokens questrade_get_tokens()
{
    questrade_tokens copy;

    pthread_mutex_lock(&questrade_tokens_mutex);
    copy = global_questrade_tokens;
    pthread_mutex_unlock(&questrade_tokens_mutex);

    return copy;
}

void questrade_set_tokens(questrade_tokens tokens)
{
    pthread_mutex_lock(&questrade_tokens_mutex);
    memcpy(&global_questrade_tokens, &tokens, sizeof(questrade_tokens));
    pthread_mutex_unlock(&questrade_tokens_mutex);
}

/*
    Accounts
*/
void questrade_set_account(questrade_account account, int index)
{
    pthread_mutex_lock(&questrade_accounts_mutex);
    global_accounts[index] = account;
    pthread_mutex_unlock(&questrade_accounts_mutex);
}

void questrade_set_all_accounts(questrade_account *accounts, int count)
{
    if (!accounts)
        return;
    pthread_mutex_lock(&questrade_accounts_mutex);
    if (global_accounts && count != global_accounts_count)
        global_accounts = realloc(global_accounts, sizeof(questrade_account) * count);
    else if (!global_accounts)
        global_accounts = malloc(sizeof(questrade_account) * count);
    global_accounts_count = count;
    memcpy(global_accounts, accounts, sizeof(questrade_account) * count);
    pthread_mutex_unlock(&questrade_accounts_mutex);
}

int questrade_get_accounts_count()
{
    int count;

    pthread_mutex_lock(&questrade_accounts_mutex);
    count = global_accounts_count;
    pthread_mutex_unlock(&questrade_accounts_mutex);
    return count;
}

questrade_account questrade_get_account(int index)
{
    questrade_account account = { 0 };

    pthread_mutex_lock(&questrade_accounts_mutex);
    account = global_accounts[index];
    pthread_mutex_unlock(&questrade_accounts_mutex);

    return account;
}

/*
    Balances
*/
void questrade_set_balance(questrade_balance balance, int index)
{
    pthread_mutex_lock(&questrade_balances_mutex);
    global_balances[index] = balance;
    pthread_mutex_unlock(&questrade_balances_mutex);
}

void questrade_add_balance(questrade_balance balance)
{
    pthread_mutex_lock(&questrade_balances_mutex);
    for (int i = 0; i < global_balances_count; i++)
    {
        if (global_balances[i].account_number == balance.account_number)
        {
            questrade_set_balance(balance, i);
            pthread_mutex_unlock(&questrade_balances_mutex);
            return;
        }
    }
    if (global_balances)
        global_balances = realloc(global_balances, sizeof(questrade_balance) * (global_balances_count + 1));
    else if (!global_balances)
        global_balances = malloc(sizeof(questrade_balance));

    global_balances[global_balances_count] = balance;
    global_balances_count++;
    pthread_mutex_unlock(&questrade_balances_mutex);
}

void questrade_set_all_balances(questrade_balance *balance, int count)
{
    if (!balance)
        return;
    pthread_mutex_lock(&questrade_balances_mutex);
    if (global_balances && count != global_balances_count)
        global_balances = realloc(global_balances, sizeof(questrade_balance) * count);
    else if (!global_balances)
        global_balances = malloc(sizeof(questrade_balance) * count);
    global_balances_count = count;
    memcpy(global_balances, balance, sizeof(questrade_balance) * count);
    pthread_mutex_unlock(&questrade_balances_mutex);
}

int questrade_get_balances_count()
{
    int count;

    pthread_mutex_lock(&questrade_balances_mutex);
    count = global_balances_count;
    pthread_mutex_unlock(&questrade_balances_mutex);
    return count;
}

questrade_balance questrade_get_balance(int index)
{
    questrade_balance account = { 0 };

    pthread_mutex_lock(&questrade_balances_mutex);
    account = global_balances[index];
    pthread_mutex_unlock(&questrade_balances_mutex);

    return account;
}

/*
    Positions
*/
void questrade_set_all_positions(questrade_position *positions, int count)
{
    if (!positions)
        return;
    pthread_mutex_lock(&questrade_positions_mutex);
    if (global_positions && count != global_positions_count)
        global_positions = realloc(global_positions, sizeof(questrade_position) * count);
    else if (!global_positions)
        global_positions = malloc(sizeof(questrade_position) * count);
    global_positions_count = count;
    memcpy(global_positions, positions, sizeof(questrade_position) * count);
    pthread_mutex_unlock(&questrade_positions_mutex);
}

int questrade_get_positions_count()
{
    int count;

    pthread_mutex_lock(&questrade_positions_mutex);
    count = global_positions_count;
    pthread_mutex_unlock(&questrade_positions_mutex);
    return count;
}

questrade_position questrade_get_position(int index)
{
    questrade_position position = { 0 };

    pthread_mutex_lock(&questrade_positions_mutex);
    position = global_positions[index];
    pthread_mutex_unlock(&questrade_positions_mutex);

    return position;
}

/*
    Quotes
*/
void questrade_set_all_quotes(questrade_quote *quotes, int count)
{
    if (!quotes)
        return;
    pthread_mutex_lock(&questrade_quotes_mutex);
    if (global_quotes && count != global_quotes_count)
        global_quotes = realloc(global_quotes, sizeof(questrade_quote) * count);
    else if (!global_quotes)
        global_quotes = malloc(sizeof(questrade_quote) * count);
    global_quotes_count = count;
    memcpy(global_quotes, quotes, sizeof(questrade_quote) * count);
    pthread_mutex_unlock(&questrade_quotes_mutex);
}
void questrade_set_quote(questrade_quote quote, int index)
{
    pthread_mutex_lock(&questrade_quotes_mutex);
    global_quotes[index] = quote;
    pthread_mutex_unlock(&questrade_quotes_mutex);
}

void questrade_add_quote(questrade_quote quote)
{
    pthread_mutex_lock(&questrade_quotes_mutex);
    for (int i = 0; i < global_quotes_count; i++)
    {
        if (global_quotes[i].symbolId == quote.symbolId)
        {
            questrade_set_quote(quote, i);
            pthread_mutex_unlock(&questrade_quotes_mutex);
            return;
        }
    }
    if (global_quotes)
        global_quotes = realloc(global_quotes, sizeof(questrade_quote) * (global_quotes_count + 1));
    else if (!global_quotes)
        global_quotes = malloc(sizeof(questrade_quote));

    global_quotes[global_quotes_count] = quote;
    global_quotes_count++;
    pthread_mutex_unlock(&questrade_quotes_mutex);
}

int questrade_get_quotes_count()
{
    int count;

    pthread_mutex_lock(&questrade_quotes_mutex);
    count = global_quotes_count;
    pthread_mutex_unlock(&questrade_quotes_mutex);
    return count;
}

questrade_quote questrade_get_quote(int index)
{
    questrade_quote quote = { 0 };

    pthread_mutex_lock(&questrade_quotes_mutex);
    quote = global_quotes[index];
    pthread_mutex_unlock(&questrade_quotes_mutex);

    return quote;
}

int questrade_find_quote(int symbolID, questrade_quote *quote_out)
{
    pthread_mutex_lock(&questrade_quotes_mutex);
    for (int i = 0; i < global_quotes_count; i++)
    {
        if (global_quotes[i].symbolId == symbolID)
        {
            *quote_out = global_quotes[i];
            pthread_mutex_unlock(&questrade_quotes_mutex);
            return 1;
        }
    }
    pthread_mutex_unlock(&questrade_quotes_mutex);
    return 0;
}

/*
    Cleanup
*/
void questrade_globals_cleanup()
{
    pthread_mutex_destroy(&questrade_settings_mutex);
    if (global_questrade_settings.callback_url)
        free(global_questrade_settings.callback_url);
    if (global_questrade_settings.consumer_key)
        free(global_questrade_settings.consumer_key);

    pthread_mutex_destroy(&questrade_tokens_mutex);

    pthread_mutex_destroy(&questrade_settings_mutex);
    if (global_accounts)
        free(global_accounts);
    global_accounts_count = 0;

    pthread_mutex_destroy(&questrade_balances_mutex);
    if (global_balances)
        free(global_balances);
    global_balances_count = 0;

    pthread_mutex_destroy(&questrade_positions_mutex);
    if (global_positions)
        free(global_positions);
    global_positions_count = 0;

    pthread_mutex_destroy(&questrade_quotes_mutex);
    if (global_quotes)
        free(global_quotes);
    global_quotes_count = 0;
}