#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "questrade.h"
#include "cJSON.h"

int questrade_parse_tokens(char *data, questrade_tokens *user_data)
{
    cJSON *json_root, *json_obj;

    if (!data || !data[0] || !user_data)
        return 0;

    json_root = cJSON_Parse(data);
    if (!json_root || cJSON_IsInvalid(json_root))
        return 0;

    json_obj = cJSON_GetObjectItem(json_root, "access_token");
    if (!json_obj || cJSON_IsInvalid(json_obj))
        goto FAIL;
    snprintf(user_data->access_token, sizeof(user_data->access_token), "%s", cJSON_GetStringValue(json_obj)); 

    json_obj = cJSON_GetObjectItem(json_root, "token_type");
    if (!json_obj || cJSON_IsInvalid(json_obj))
        goto FAIL;
    snprintf(user_data->token_type, sizeof(user_data->token_type), "%s", cJSON_GetStringValue(json_obj)); 

    json_obj = cJSON_GetObjectItem(json_root, "expires_in");
    if (!json_obj || cJSON_IsInvalid(json_obj))
        goto FAIL;
    user_data->expires_in = cJSON_GetNumberValue(json_obj);

    json_obj = cJSON_GetObjectItem(json_root, "refresh_token");
    if (!json_obj || cJSON_IsInvalid(json_obj))
        goto FAIL;
    snprintf(user_data->refresh_token, sizeof(user_data->refresh_token), "%s", cJSON_GetStringValue(json_obj));

    json_obj = cJSON_GetObjectItem(json_root, "api_server");
    if (!json_obj || cJSON_IsInvalid(json_obj))
        goto FAIL;
    snprintf(user_data->api_server, sizeof(user_data->api_server), "%s", cJSON_GetStringValue(json_obj));

    cJSON_Delete(json_root);
    return 1;

FAIL:
    cJSON_Delete(json_root);
    return 0;
}

int questrade_parse_accounts(char *data, questrade_account **user_data)
{
    int account_count;
    cJSON *json_root, *json_accounts, *json_obj;

    json_root = cJSON_Parse(data);
    if (!json_root)
        return 0;

    *user_data = NULL;
    json_accounts = cJSON_GetObjectItem(json_root, "accounts");
    if (!json_accounts || cJSON_IsInvalid(json_accounts))
        goto FAIL;

    account_count = cJSON_GetArraySize(json_accounts);
    *user_data = calloc(account_count, sizeof(questrade_account));

    for (int i = 0; i < account_count; i++)
    {
        cJSON *json_account = cJSON_GetArrayItem(json_accounts, i);
        if (!json_account || cJSON_IsInvalid(json_account))
            goto FAIL;

        json_obj = cJSON_GetObjectItem(json_account, "type");
        if (!json_obj || cJSON_IsInvalid(json_obj))
            goto FAIL;
        snprintf((*user_data)[i].type, sizeof((*user_data)[i].type), "%s", cJSON_GetStringValue(json_obj));

        json_obj = cJSON_GetObjectItem(json_account, "number");
        if (!json_obj || cJSON_IsInvalid(json_obj))
            goto FAIL;
        (*user_data)[i].number = strtol(cJSON_GetStringValue(json_obj), NULL, 10);

        json_obj = cJSON_GetObjectItem(json_account, "status");
        if (!json_obj || cJSON_IsInvalid(json_obj))
            goto FAIL;
        snprintf((*user_data)[i].status, sizeof((*user_data)[i].status), "%s", cJSON_GetStringValue(json_obj)); 

        json_obj = cJSON_GetObjectItem(json_account, "isPrimary");
        if (!json_obj || cJSON_IsInvalid(json_obj))
            goto FAIL;
        (*user_data)[i].isPrimary = cJSON_GetNumberValue(json_obj);

        json_obj = cJSON_GetObjectItem(json_account, "isBilling");
        if (!json_obj || cJSON_IsInvalid(json_obj))
            goto FAIL;
        (*user_data)[i].isBilling = cJSON_GetNumberValue(json_obj);

        json_obj = cJSON_GetObjectItem(json_account, "clientAccountType");
        if (!json_obj || cJSON_IsInvalid(json_obj))
            goto FAIL;
        snprintf((*user_data)[i].clientAccountType, sizeof((*user_data)[i].clientAccountType), "%s", cJSON_GetStringValue(json_obj));
    }

    cJSON_Delete(json_root);
    return account_count;
    
FAIL:
    if (*user_data)
        free(*user_data);
    cJSON_Delete(json_root);
    return 0;
}

/*
    Only parses combined balances
*/
int questrade_parse_balance(char *data, questrade_balance *user_data)
{
    cJSON *json_root, *combined, *tmp;

    json_root = cJSON_Parse(data);
    if (!json_root)
        return 0;

    combined = cJSON_GetObjectItem(json_root, "combinedBalances");
    if (!combined)
        goto FAIL;

    combined = cJSON_GetArrayItem(combined, 0);

    snprintf(user_data->currency, sizeof(user_data->currency), "%s", "combined");

    tmp = cJSON_GetObjectItem(combined, "cash");
    if (!tmp)
        goto FAIL;
    user_data->cash = cJSON_GetNumberValue(tmp);

    tmp = cJSON_GetObjectItem(combined, "marketValue");
    if (!tmp)
        goto FAIL;
    user_data->marketValue = cJSON_GetNumberValue(tmp);

    tmp = cJSON_GetObjectItem(combined, "totalEquity");
    if (!tmp)
        goto FAIL;
    user_data->totalEquity = cJSON_GetNumberValue(tmp);

    tmp = cJSON_GetObjectItem(combined, "buyingPower");
    if (!tmp)
        goto FAIL;
    user_data->buyingPower = cJSON_GetNumberValue(tmp);

    tmp = cJSON_GetObjectItem(combined, "maintenanceExcess");
    if (!tmp)
        goto FAIL;
    user_data->maintenanceExcess = cJSON_GetNumberValue(tmp);

    tmp = cJSON_GetObjectItem(combined, "isRealTime");
    if (!tmp)
        goto FAIL;
    user_data->isRealTime = tmp->valueint;

    cJSON_Delete(json_root);
    return 1;

FAIL:
    cJSON_Delete(json_root);
    return 0;
}

int questrade_parse_positions(char *data, questrade_position **user_data)
{
    int positions_count;
    cJSON *json_root, *json_positions, *json_obj;

    json_root = cJSON_Parse(data);
    if (!json_root)
        return 0;

    json_positions = cJSON_GetObjectItem(json_root, "positions");
    if (!json_positions || cJSON_IsInvalid(json_positions))
        goto FAIL;

    positions_count = cJSON_GetArraySize(json_positions);
    if (positions_count == 0) /* technically valid */ 
        goto FAIL;
    *user_data = calloc(positions_count, sizeof(questrade_position));

    for (int i = 0; i < positions_count; i++)
    {
        cJSON *json_position = cJSON_GetArrayItem(json_positions, i);

        json_obj = cJSON_GetObjectItem(json_position, "symbol");
        if (!json_obj)
            goto FAIL;
        snprintf((*user_data)[i].symbol, sizeof((*user_data)[i].symbol), "%s", cJSON_GetStringValue(json_obj));

        json_obj = cJSON_GetObjectItem(json_position, "symbolId");
        if (!json_obj)
            goto FAIL;
        (*user_data)[i].symbolId = cJSON_GetNumberValue(json_obj);

        json_obj = cJSON_GetObjectItem(json_position, "openQuantity");
        if (!json_obj)
            goto FAIL;
        (*user_data)[i].openQuantity = cJSON_GetNumberValue(json_obj);

        json_obj = cJSON_GetObjectItem(json_position, "currentMarketValue");
        if (!json_obj)
            goto FAIL;
        (*user_data)[i].currentMarketValue = cJSON_GetNumberValue(json_obj);

        json_obj = cJSON_GetObjectItem(json_position, "currentPrice");
        if (!json_obj)
            goto FAIL;
        (*user_data)[i].currentPrice = cJSON_GetNumberValue(json_obj);

        json_obj = cJSON_GetObjectItem(json_position, "averageEntryPrice");
        if (!json_obj)
            goto FAIL;
        (*user_data)[i].averageEntryPrice = cJSON_GetNumberValue(json_obj);

        json_obj = cJSON_GetObjectItem(json_position, "closedPnl");
        if (!json_obj)
            goto FAIL;
        (*user_data)[i].closedPnl = cJSON_GetNumberValue(json_obj);
        
        json_obj = cJSON_GetObjectItem(json_position, "dayPnl");
        if (!json_obj)
            goto FAIL;
        (*user_data)[i].dayPnl = cJSON_GetNumberValue(json_obj);

        json_obj = cJSON_GetObjectItem(json_position, "openPnl");
        if (!json_obj)
            goto FAIL;
        (*user_data)[i].closedPnl = cJSON_GetNumberValue(json_obj);

        json_obj = cJSON_GetObjectItem(json_position, "totalCost");
        if (!json_obj)
            goto FAIL;
        (*user_data)[i].totalCost = cJSON_GetNumberValue(json_obj);

        json_obj = cJSON_GetObjectItem(json_position, "isRealTime");
        if (!json_obj)
            goto FAIL;
        (*user_data)[i].isRealTime = cJSON_GetNumberValue(json_obj);
    
        json_obj = cJSON_GetObjectItem(json_position, "isUnderReorg");
        if (!json_obj)
            goto FAIL;
        (*user_data)[i].isUnderReorg = cJSON_GetNumberValue(json_obj);
    }
    
    cJSON_Delete(json_root);
    return positions_count;

FAIL:
    cJSON_Delete(json_root);
    return 0;
}

int questrade_parse_quote(char *data, questrade_quote **user_data)
{
    int quote_count;
    cJSON *json_root, *json_symbols, *json_tmp;

    json_root = cJSON_Parse(data);
    if (!json_root)
        return 0;

    *user_data = NULL;
    json_symbols = cJSON_GetObjectItem(json_root, "symbols");
    if (!json_symbols)
        goto FAIL;
    quote_count = cJSON_GetArraySize(json_symbols);
    *user_data = malloc(sizeof(questrade_quote) * quote_count);

    for (int i = 0; i < quote_count; i++)
    {
        cJSON *json_symbol = cJSON_GetArrayItem(json_symbols, i);

        json_tmp = cJSON_GetObjectItem(json_symbol, "symbol");
        if (!json_tmp)
            goto FAIL;
        snprintf((*user_data)[i].symbol, sizeof((*user_data)[i].symbol), "%s", cJSON_GetStringValue(json_tmp));
        json_tmp = cJSON_GetObjectItem(json_symbol, "symbolId");
        if (!json_tmp)
            goto FAIL;
        (*user_data)[i].symbolId = cJSON_GetNumberValue(json_tmp);
        json_tmp = cJSON_GetObjectItem(json_symbol, "prevDayClosePrice");
        if (!json_tmp)
            goto FAIL;
        (*user_data)[i].prevDayClosePrice = cJSON_GetNumberValue(json_tmp);
        json_tmp = cJSON_GetObjectItem(json_symbol, "highPrice52");
        if (!json_tmp)
            goto FAIL;
        (*user_data)[i].highPrice52 = cJSON_GetNumberValue(json_tmp);
        json_tmp = cJSON_GetObjectItem(json_symbol, "lowPrice52");
        if (!json_tmp)
            goto FAIL;
        (*user_data)[i].lowPrice52 = cJSON_GetNumberValue(json_tmp);
        json_tmp = cJSON_GetObjectItem(json_symbol, "averageVol3Months");
        if (!json_tmp)
            goto FAIL;
        (*user_data)[i].averageVol3Months = cJSON_GetNumberValue(json_tmp);
        json_tmp = cJSON_GetObjectItem(json_symbol, "averageVol20Days");
        if (!json_tmp)
            goto FAIL;
        (*user_data)[i].averageVol20Days = cJSON_GetNumberValue(json_tmp);
        json_tmp = cJSON_GetObjectItem(json_symbol, "outstandingShares");
        if (!json_tmp)
            goto FAIL;
        (*user_data)[i].outstandingShares = cJSON_GetNumberValue(json_tmp);
        json_tmp = cJSON_GetObjectItem(json_symbol, "eps");
        if (!json_tmp)
            goto FAIL;
        (*user_data)[i].eps = cJSON_GetNumberValue(json_tmp);
        json_tmp = cJSON_GetObjectItem(json_symbol, "pe");
        (*user_data)[i].pe = cJSON_GetNumberValue(json_tmp);
        json_tmp = cJSON_GetObjectItem(json_symbol, "dividend");
        if (!json_tmp)
            goto FAIL;
        (*user_data)[i].dividend = cJSON_GetNumberValue(json_tmp);
        json_tmp = cJSON_GetObjectItem(json_symbol, "yield");
        if (!json_tmp)
            goto FAIL;
        (*user_data)[i].yield = cJSON_GetNumberValue(json_tmp);

        json_tmp = cJSON_GetObjectItem(json_symbol, "exDate");
        if (json_tmp->valuestring)
            snprintf((*user_data)[i].exDate, sizeof((*user_data)[i].symbol), "%s", cJSON_GetStringValue(json_tmp));
        else
            (*user_data)[i].exDate[0] = 0;

        json_tmp = cJSON_GetObjectItem(json_symbol, "marketCap");
        if (!json_tmp)
            goto FAIL;
        (*user_data)[i].marketCap = cJSON_GetNumberValue(json_tmp);
        json_tmp = cJSON_GetObjectItem(json_symbol, "tradeUnit");
        if (!json_tmp)
            goto FAIL;
        (*user_data)[i].tradeUnit = cJSON_GetNumberValue(json_tmp);
        json_tmp = cJSON_GetObjectItem(json_symbol, "listingExchange");
        if (!json_tmp)
            goto FAIL;
        snprintf((*user_data)[i].listingExchange, sizeof((*user_data)[i].symbol), "%s", cJSON_GetStringValue(json_tmp));
        json_tmp = cJSON_GetObjectItem(json_symbol, "description");
        if (!json_tmp)
            goto FAIL;
        snprintf((*user_data)[i].description, sizeof((*user_data)[i].symbol), "%s", cJSON_GetStringValue(json_tmp));
        json_tmp = cJSON_GetObjectItem(json_symbol, "securityType");
        if (!json_tmp)
            goto FAIL;
        snprintf((*user_data)[i].securityType, sizeof((*user_data)[i].symbol), "%s", cJSON_GetStringValue(json_tmp));
        
        json_tmp = cJSON_GetObjectItem(json_symbol, "dividendDate");
        if (json_tmp->valuestring)
            snprintf((*user_data)[i].dividendDate,  sizeof((*user_data)[i].symbol), "%s", cJSON_GetStringValue(json_tmp));
        else
            (*user_data)[i].dividendDate[0] = 0;

        json_tmp = cJSON_GetObjectItem(json_symbol, "isTradable");
        if (!json_tmp)
            goto FAIL;
        (*user_data)[i].isTradable = cJSON_GetNumberValue(json_tmp);
        json_tmp = cJSON_GetObjectItem(json_symbol, "isQuotable");
        if (!json_tmp)
            goto FAIL;
        (*user_data)[i].isQuotable = cJSON_GetNumberValue(json_tmp);
        json_tmp = cJSON_GetObjectItem(json_symbol, "hasOptions");
        if (!json_tmp)
            goto FAIL;
        (*user_data)[i].hasOptions = cJSON_GetNumberValue(json_tmp);
    }

    cJSON_Delete(json_root);
    return quote_count;

FAIL:
    if (*user_data)
        free(*user_data);
    cJSON_Delete(json_root);
    return 0;
}

int questrade_parse_candle(char *data, questrade_candle **user_data)
{
    int count;
    cJSON *json_root, *json_candles, *json_tmp;

    json_root = cJSON_Parse(data);
    if (!json_root)
        return 0;

    *user_data = NULL;
    json_candles = cJSON_GetObjectItem(json_root, "candles");
    if (!json_candles)
        goto FAIL_CANDLES;
    count = cJSON_GetArraySize(json_candles);
    if (count == 0)
        goto FAIL_CANDLES;
    *user_data = malloc(sizeof(questrade_candle) * count);

    for (int i = 0; i < count; i++)
    {
        cJSON *json_candle = cJSON_GetArrayItem(json_candles, i);
        if (!json_candle)
            goto FAIL_CANDLES;

        json_tmp = cJSON_GetObjectItem(json_candle, "start");
        if (!json_tmp)
            goto FAIL_CANDLES;
        snprintf((*user_data)[i].start, sizeof((*user_data)[i].start), "%s", cJSON_GetStringValue(json_tmp));
        json_tmp = cJSON_GetObjectItem(json_candle, "end");
        if (!json_tmp)
            goto FAIL_CANDLES;
        snprintf((*user_data)[i].end, sizeof((*user_data)[i].end), "%s", cJSON_GetStringValue(json_tmp));
        json_tmp = cJSON_GetObjectItem(json_candle, "low");
        if (!json_tmp)
            goto FAIL_CANDLES;
        (*user_data)[i].low = cJSON_GetNumberValue(json_tmp);
        json_tmp = cJSON_GetObjectItem(json_candle, "high");
        if (!json_tmp)
            goto FAIL_CANDLES;
        (*user_data)[i].high = cJSON_GetNumberValue(json_tmp);
        json_tmp = cJSON_GetObjectItem(json_candle, "open");
        if (!json_tmp)
            goto FAIL_CANDLES;
        (*user_data)[i].open = cJSON_GetNumberValue(json_tmp);
        json_tmp = cJSON_GetObjectItem(json_candle, "close");
        if (!json_tmp)
            goto FAIL_CANDLES;
        (*user_data)[i].close = cJSON_GetNumberValue(json_tmp);
        json_tmp = cJSON_GetObjectItem(json_candle, "volume");
        if (!json_tmp)
            goto FAIL_CANDLES;
        (*user_data)[i].volume = cJSON_GetNumberValue(json_tmp);
    }

    cJSON_Delete(json_root);
    return count;
    
FAIL_CANDLES:
    if (*user_data)
        free(user_data);
    cJSON_Delete(json_root);
    return 0;
}
