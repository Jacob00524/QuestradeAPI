#include <stdlib.h>
#include <stdio.h>

#include "questrade.h"
#include "curlwrapper.h"

/*
    user must free the pointer where accounts_out points to
*/
int questrade_fetch_accounts(questrade_account **accounts_out, char **json_out)
{
    int account_count;
    long response_status;
    char url[1024], *response;
    questrade_tokens tokens;

    if (!accounts_out)
        return 0;

    tokens = questrade_get_tokens();
    snprintf(url, sizeof(url), "%sv1/accounts", tokens.api_server);
    response = http_get(url, tokens.access_token, &response_status);
    if (response_status != 200 || !response)
    {
        if (response)
            free(response);
        return 0;
    }

    if (!(account_count = questrade_parse_accounts(response, accounts_out)))
    {
        free(response);
        return 0;
    }

    if (json_out)
        *json_out = response;
    else
        free(response);

    return account_count;
}

int questrade_update_accounts(char **json_out)
{
    int count;
    questrade_account *accounts;

    if (!(count = questrade_fetch_accounts(&accounts, json_out)))
        return 0;

    questrade_set_all_accounts(accounts, count);
    free(accounts);
    return count;
}

int questrade_fetch_balance(questrade_balance *balance_out, unsigned long account_number, char **json_out)
{
    long response_status;
    char url[1024], *response;
    questrade_tokens tokens;

    if (!balance_out)
        return 0;

    tokens = questrade_get_tokens();
    sprintf(url, "%sv1/accounts/%lu/balances", tokens.api_server, account_number);
    
    response = http_get(url, tokens.access_token, &response_status);
    if (response_status != 200 || !response)
    {
        if (response)
            free(response);
        return 0;
    }

    if (!questrade_parse_balance(response, balance_out))
    {
        free(response);
        return 0;
    }
    balance_out->account_number = account_number;

    if (json_out)
        *json_out = response;
    else
        free(response);

    return 1;
}

int questrade_update_balance(unsigned long account_number, char **json_out)
{
    questrade_balance balance;

    if (!questrade_fetch_balance(&balance, account_number, json_out))
        return 0;

    questrade_add_balance(balance);
    return 1;
}

int questrade_fetch_positions(questrade_position **positions_out, unsigned long account_number, char **json_out)
{
    long response_status;
    char url[1024], *response;
    int positions_count;
    questrade_tokens tokens;

    if (!positions_out)
        return 0;

    tokens = questrade_get_tokens();

    sprintf(url, "%sv1/accounts/%lu/positions", tokens.api_server, account_number);
    response = http_get(url, tokens.access_token, &response_status);

    if (response_status != 200 || !response)
    {
        if (response)
            free(response);
        return 0;
    }

    if (!(positions_count = questrade_parse_positions(response, positions_out)))
    {
        free(response);
        return 0;
    }

    if (json_out)
        *json_out = response;
    else
        free(response);

    return positions_count;
}

int questrade_update_positions(unsigned long account_number, char **json_out)
{
    int count;
    questrade_position *positions;

    if (!(count = questrade_fetch_positions(&positions, account_number, json_out)))
        return 0;

    questrade_set_all_positions(positions, count);
    free(positions);
    return count;
}

int questrade_fetch_quote(questrade_quote **quote_out, char *ids, char **json_out)
{
    long response_status;
    char url[1024], *response;
    int count;
    questrade_tokens tokens;

    if (!quote_out || !ids)
        return 0;

    tokens = questrade_get_tokens();

    sprintf(url, "%sv1/symbols?ids=%s", tokens.api_server, ids);
    response = http_get(url, tokens.access_token, &response_status);

    if (response_status != 200 || !response)
    {
        if (response)
            free(response);
        return 0;
    }

    if (!(count = questrade_parse_quote(response, quote_out)))
    {
        free(response);
        return 0;
    }

    if (json_out)
        *json_out = response;
    else
        free(response);

    return count;
}
int questrade_update_quotes(char *ids, char **json_out)
{
    int count;
    questrade_quote *quotes;

    if (!ids)
        return 0;
    if (!(count = questrade_fetch_quote(&quotes, ids, json_out)))
        return 0;
    
    for (int i = 0; i < count; i++)
        questrade_add_quote(quotes[i]);

    free(quotes);
    return count;
}

int questrade_fetch_candle(questrade_candle **candles_out, char **json_out, unsigned int symbolId, char *start, char *end, char *interval)
{
    long response_status;
    char url[1024], *response;
    int count;
    questrade_tokens tokens = questrade_get_tokens();

    sprintf(url, "%sv1/markets/candles/%u?startTime=%s&endTime=%s&interval=%s", tokens.api_server, symbolId, start, end, interval);
    response = http_get(url, tokens.access_token, &response_status);

    if (response_status != 200 || !response)
    {
        if (response)
            free(response);
        return 0;
    }

    if (!(count = questrade_parse_candle(response, candles_out)))
    {
        free(response);
        return 0;
    }

    if (json_out)
        *json_out = response;
    else
        free(response);

    return count;
}