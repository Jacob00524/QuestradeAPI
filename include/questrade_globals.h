questrade_settings questrade_get_settings();
void questrade_set_settings(questrade_settings settings);

questrade_tokens questrade_get_tokens();
void questrade_set_tokens(questrade_tokens tokens);

void questrade_set_account(questrade_account account, int index);
void questrade_set_all_accounts(questrade_account *accounts, int count);
int questrade_get_accounts_count();
questrade_account questrade_get_account(int index);

void questrade_set_balance(questrade_balance balance, int index);
void questrade_add_balance(questrade_balance balance);
void questrade_set_all_balances(questrade_balance *balance, int count);
int questrade_get_balances_count();
questrade_balance questrade_get_balance(int index);

void questrade_set_all_positions(questrade_position *positions, int count);
int questrade_get_positions_count();
questrade_position questrade_get_position(int index);

void questrade_set_all_quotes(questrade_quote *quotes, int count);
void questrade_set_quote(questrade_quote quote, int index);
void questrade_add_quote(questrade_quote quote);
int questrade_get_quotes_count();
questrade_quote questrade_get_quote(int index);
/*
    looks for a quote with a matching symbolID in the global quotes
    returns 1 if found and populates quote_out
    returns 0 if not found
*/
int questrade_find_quote(int symbolID, questrade_quote *quote_out);


void questrade_globals_cleanup();
