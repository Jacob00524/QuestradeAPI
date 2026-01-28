/* Signals */
#define NO_SIGNAL 1
#define BUY 2
#define SELL 3

int questrade_algorithm_stats_compare(const void *a, const void *b);

/*
    Crossover
*/
questrade_crossover_settings questrade_get_crossover_settings();
void questrade_set_crossover_settings(questrade_crossover_settings settings);
int questrade_calc_ma_crossover(questrade_candle *candles, int count, int slow_period, int fast_period, int *out_index, int *out_signal, int out_max);
questrade_algorithm_stats questrade_calc_ma_crossover_profit(questrade_candle *candles, int count, int *signal_index, int *signals, int signals_count);
questrade_algorithm_stats questrade_find_best_ma_crossover(questrade_candle *candles, int candles_count, int *slow_period_out, int *fast_period_out);
cJSON *questrade_crossover_analyze_run_tsx30_sector(questrade_TSX_30_Stock *stocks, int tsx30_count);
