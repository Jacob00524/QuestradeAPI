int questrade_login_from_code(char *code);
int questrade_login_from_refresh(char *token);

int questrade_start_login_routine(questrade_tokens current_tokens);
void questrade_stop_login_routine();

void questrade_login_cleanup();
