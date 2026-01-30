#include <time.h>
#include <stdlib.h>
#include <stdio.h>

#include "questrade.h"

int main()
{
    int accounts_count, position_count, quotes_count, candles_count;
    questrade_tokens tokens;
    questrade_account account;
    questrade_balance balance;
    questrade_candle *candles = NULL;
    struct tm start_tm = { 0 }, end_tm = { 0 };
    char start_str[50], end_str[50];

    if (!questrade_init(NULL, NULL, NULL))
    {
        printf("Failed to initialize questrade library.\n");
        return 1;
    }

    if (!questrade_login_from_refresh("refresh-token")) /* Insert refresh token from app hub, this is an example */
    {
        printf("Failed to login using refresh token.\n");
        return 1;
    }

    tokens = questrade_get_tokens();

    if (!questrade_start_login_routine(tokens))
    {
        printf("Failed to start login routine.\n");
        questrade_cleanup();
        return 1;
    }

    accounts_count = questrade_update_accounts(NULL);
    if (accounts_count == 0)
    {
        printf("No accounts.\n");
        questrade_cleanup();
        return 0;
    }
    account = questrade_get_account(0);
    printf("Account Number: %lu\nAccount Type: %s\nType: %s\n", account.number, account.type, account.clientAccountType);

    if (!questrade_update_balance(account.number, NULL))
    {
        printf("No balances.\n");
        questrade_cleanup();
        return 0;
    }
    balance = questrade_get_balance(0);
    printf("Cash: %lf\nMarket Value: %lf\nTotal Equity: %lf\n", balance.cash, balance.marketValue, balance.totalEquity);

    printf("Your positions....\n");
    position_count = questrade_update_positions(account.number, NULL);
    if (position_count == 0)
    {
        printf("No positions.\n");
        questrade_cleanup();
        return 0;
    }
    for (int i = 0; i < position_count; i++)
    {
        questrade_position position = questrade_get_position(i);
        printf("%s:\n\tID: %d\n\tQty: %lf\n\tPrice Per: %lf\n\tPrice Total: %lf\n", position.symbol, position.symbolId, position.openQuantity, position.currentPrice, position.currentMarketValue);
    }

    printf("Some quotes....\n");
    quotes_count = questrade_update_quotes("26777456,9339", NULL);
    if (quotes_count == 0)
    {
        printf("No quotes returned.\n");
        questrade_cleanup();
        return 0;
    }else if (quotes_count != 2)
        printf("Unexpected number of quotes returned.\n");
    for (int i = 0; i < quotes_count; i++)
    {
        questrade_quote quote = questrade_get_quote(i);
        printf("%s:\n\tID: %d\n\tPrev Closing: %lf\n\tOutstanding Shares: %lu\n\tExchange: %s\n", quote.symbol, quote.symbolId, quote.prevDayClosePrice, quote.outstandingShares, quote.listingExchange);
    }

    printf("A day of candles:\n");
    start_tm.tm_year = 2026 - 1900; /* very important to subtract 1900, don't forget */
    start_tm.tm_mon = 0; /* January */
    start_tm.tm_mday = 27;
    questrade_tm_to_iso(&start_tm, start_str, sizeof(start_str));

    end_tm.tm_year = 2026 - 1900;
    end_tm.tm_mon = 0;
    end_tm.tm_mday = 28;
    questrade_tm_to_iso(&end_tm, end_str, sizeof(end_str));

    candles_count = questrade_fetch_candle(&candles, NULL, 9339, start_str, end_str, "OneHour");
    if (candles_count == 0 || !candles)
    {
        printf("No candles.\n");
        if (candles)
            free(candles);
        questrade_cleanup();
        return 0;
    }
    for (int i = 0; i < candles_count; i++)
        printf("%lf%s", candles[i].close, i < (candles_count - 1) ? "," : "\n");

    free(candles);
    questrade_cleanup();
    return 0;
}
