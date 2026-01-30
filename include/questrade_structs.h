#include <time.h>

typedef struct questrade_tokens
{
    char access_token[256];
    char token_type[128];
    int expires_in;
    char refresh_token[256];
    char api_server[256];
    time_t time_refreshed;
}questrade_tokens;

typedef struct questrade_settings
{
    char *consumer_key; /* this is also the client_id */
    char *callback_url; /* used for login with code */
    void(*token_refresh_callback)(questrade_tokens);
}questrade_settings;

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

/*
    Algorithms structs
*/
typedef struct questrade_algorithm_stats
{
    int wins;
    int losses;
    int draws;
    int trade_count;
    int buy_count;
    int sell_count;
    time_t last_calculated;
    double profit_per_stock;
    double gross_profit;
    double gross_loss;
    double profit_factor;
    double max_drawdown;
    double avg_trade_pnl;
}questrade_algorithm_stats;

typedef struct questrade_crossover_stats
{
    int fast_period;
    int slow_period;
    char symbol[256];
    int symbolId;
    questrade_algorithm_stats stats;
}questrade_crossover_stats;

typedef struct questrade_crossover_settings
{
    int allow_loss;
    int min_trade_count;
    int step;
}questrade_crossover_settings;


/* 
    TSX30
*/
typedef struct questrade_TSX_30_Stock
{
    int ranking;
    char name[1024];
    char symbol[256];
    int sector;
    int questrade_id;
}questrade_TSX_30_Stock;

typedef struct questrade_TSX_30
{
    int technology_count;
    questrade_TSX_30_Stock *technology;

    int mining_count;
    questrade_TSX_30_Stock *mining;

    int indsutrial_products_services_count;
    questrade_TSX_30_Stock *indsutrial_products_services;

    int clean_technology_energy_count;
    questrade_TSX_30_Stock *clean_technology_energy;

    int financial_services_count;
    questrade_TSX_30_Stock *financial_services;
}questrade_TSX_30;
