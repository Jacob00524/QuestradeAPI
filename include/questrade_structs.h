#include <time.h>

typedef struct questrade_settings
{
    char *consumer_key; /* this is also the client_id */
    char *callback_url; /* used for login with code */
}questrade_settings;

typedef struct questrade_tokens
{
    char access_token[256];
    char token_type[128];
    int expires_in;
    char refresh_token[256];
    char api_server[256];
    time_t time_refreshed;
}questrade_tokens;

typedef struct questrade_account
{
    char type[128];
    char status[128];
    int isPrimary;
    int isBilling;
    unsigned long number;
    char clientAccountType[256];
}questrade_account;

typedef struct questrade_balance
{
    char currency[20];
    double cash;
    double marketValue;
    double totalEquity;
    double buyingPower;
    double maintenanceExcess;
    int isRealTime;
    unsigned long account_number;
}questrade_balance;

typedef struct questrade_position
{
    char symbol[256];
    int symbolId;
    double openQuantity;
    double currentMarketValue;
    double currentPrice;
    double averageEntryPrice;
    double closedPnl;
    double openPnl;
    double dayPnl;
    int totalCost;
    int isRealTime;
    int isUnderReorg;
}questrade_position;

typedef struct questrade_quote
{
    char symbol[256];
    int symbolId;
    double prevDayClosePrice;
    double highPrice52;
    double lowPrice52;
    unsigned long averageVol3Months;
    unsigned long averageVol20Days;
    unsigned long outstandingShares;
    double eps;
    double pe;
    double dividend;
    double yield;
    char exDate[1024];
    double marketCap;
    int tradeUnit;
    char listingExchange[256];
    char description[1024];
    char securityType[256];
    char dividendDate[1024];
    int isTradable;
    int isQuotable;
    int hasOptions;
}questrade_quote;

typedef struct questrade_candle
{
    char start[1024];
    char end[1024];
    double low;
    double high;
    double open;
    double close;
    unsigned long volume;
}questrade_candle;
