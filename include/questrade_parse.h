int questrade_parse_tokens(char *data, questrade_tokens *user_data);
int questrade_parse_accounts(char *data, questrade_account **user_data);
int questrade_parse_balance(char *data, questrade_balance *user_data);
int questrade_parse_positions(char *data, questrade_position **user_data);
int questrade_parse_quote(char *data, questrade_quote **user_data);
int questrade_parse_candle(char *data, questrade_candle **user_data);
