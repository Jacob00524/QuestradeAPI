/*
    Queries the questrade servers for accounts
*/
int questrade_fetch_accounts(questrade_account **accounts_out, char **json_out);
/*
    fetches accounts and updates the accounts global variables
    returns number of accounts
*/
int questrade_update_accounts(char **json_out); 

/*     
    Queries the questrade servers for an accounts balance
*/
int questrade_fetch_balance(questrade_balance *balances_out, unsigned long account_number, char **json_out);
/*
    fetches an account balance and adds it to the balances global variables
    if a balance of that account already exists, then it is updated.
*/
int questrade_update_balance(unsigned long account_number, char **json_out);

/*
    Queries the questrade servers for an accounts positions
*/
int questrade_fetch_positions(questrade_position **positions_out, unsigned long account_number, char **json_out);
/*
    fetches an accounts positions and updates the position global variables
    returns number of positions
*/
int questrade_update_positions(unsigned long account_number, char **json_out);

/*
    Queries the questrade servers for a quote of a symbol
*/
int questrade_fetch_quote(questrade_quote **quote_out, char *ids, char **json_out);
/*
    fetches specified quotes and updates those quotes in global quotes variable
    returns number of quotes
*/
int questrade_update_quotes(char *ids, char **json_out);

/*
    Queries the questrade servers for candles

    start and end must be in ISO 8601

    Interval can be:
    OneMinute, TwoMinutes, ThreeMinutes, FourMinutes, FiveMinutes, 
    TenMinutes, FifteenMinutes, TwentyMinutes, HalfHour, OneHour, TwoHours, 
    FourHours, OneDay, OneWeek, OneMonth, OneYear

*/
int questrade_fetch_candle(questrade_candle **candles_out, char **json_out, unsigned int symbolId, char *start, char *end, char *interval);
