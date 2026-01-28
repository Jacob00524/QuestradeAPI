#include "questrade.h"

/*
    returns number of signals
*/
int questrade_calc_ma_crossover(questrade_candle *candles, int count, int slow_period, int fast_period, int *out_index, int *out_signal, int out_max)
{
    double *fast_ma, *slow_ma;
    int fast_count, slow_count, found_count = 0, n;

    fast_count = questrade_calc_moving_series(candles, count, fast_period, &fast_ma);
    if (fast_count <= 0)
        return 0;
    slow_count = questrade_calc_moving_series(candles, count, slow_period, &slow_ma);
    if (slow_count <= 0)
        return 0;

    n = (fast_count < slow_count) ? fast_count : slow_count;

    /* check for crossover */
    for (int i = 1; i < n; i++) 
    {
        double fast_now  = fast_ma[i - 1];
        double slow_now  = slow_ma[i - 1];

        double fast_prev = fast_ma[i];
        double slow_prev = slow_ma[i];

        if (fast_prev <= slow_prev && fast_now > slow_now)
        {
            if (found_count < out_max)
            {
                out_index[found_count] = (count - 1) - (i - 1); /* index of signal candle */
                out_signal[found_count] = BUY;
                found_count++;
            }
        }
        else if (fast_prev >= slow_prev && fast_now < slow_now)
        {
            if (found_count < out_max)
            {
                out_index[found_count] = (count - 1) - (i - 1);
                out_signal[found_count] = SELL;
                found_count++;
            }
        }
    }

    return found_count;
}

static inline void update_dd(double equity, double *peak, double *maxdd)
{
    if (equity > *peak) *peak = equity;
    double dd = *peak - equity;
    if (dd > *maxdd) *maxdd = dd;
}

questrade_algorithm_stats questrade_calc_ma_crossover_profit(questrade_candle *candles, int count, int *signal_index, int *signals, int signals_count)
{
    double profit = 0, price_bought = 0;
    double peak_equity = 0, max_drawdown = 0;
    int in_position = 0;
    int last_idx = -1;

    questrade_algorithm_stats s = {0};
    s.last_calculated = time(NULL);

    for (int i = 0; i < signals_count; i++)
    {
        int idx = signal_index[i];
        int sig = signals[i];
        if (idx < 0 || idx >= count) continue;

        last_idx = idx;
        double price = candles[idx].close;

        if (sig == BUY && !in_position)
        {
            profit -= price;
            in_position = 1;
            price_bought = price;

            s.buy_count++;
            update_dd(profit, &peak_equity, &max_drawdown);
        }
        else if (sig == SELL && in_position)
        {
            double delta = price - price_bought;

            if (delta > 0) { s.wins++;   s.gross_profit +=  delta; }
            else if (delta < 0) 
            {
                questrade_crossover_settings settings = questrade_get_crossover_settings();

                if (settings.allow_loss)
                {
                    s.losses++;
                    s.gross_loss += -delta;
                }
                else
                    continue;
            }
            else { s.draws++; }

            profit += price;
            in_position = 0;

            s.sell_count++;
            s.trade_count++;

            update_dd(profit, &peak_equity, &max_drawdown);
        }
    }

    if (in_position)
    {
        double final_price;
        if (last_idx >= 0) final_price = candles[last_idx].close;
        else final_price = candles[0].close;

        double delta = final_price - price_bought;
        if (delta > 0) { s.wins++; s.gross_profit += delta; }
        else if (delta < 0) { s.losses++; s.gross_loss += -delta; }
        else { s.draws++; }

        profit += final_price;
        in_position = 0;

        s.sell_count++;
        s.trade_count++;

        update_dd(profit, &peak_equity, &max_drawdown);
    }

    s.profit_per_stock = profit;
    s.max_drawdown = max_drawdown;

    if (s.gross_loss > 0)
        s.profit_factor = s.gross_profit / s.gross_loss;
    else if (s.gross_profit > 0)
        s.profit_factor = INFINITY;
    else
        s.profit_factor = 0;

    if (s.trade_count > 0)
        s.avg_trade_pnl = profit / (double)s.trade_count;
    else
        s.avg_trade_pnl = 0;

    return s;
}

static double score_stats(const questrade_algorithm_stats *s)
{
    questrade_crossover_settings settings = questrade_get_crossover_settings();
    if (s->trade_count < settings.min_trade_count)
        return -DBL_MAX; /* reject if too few trades */
    return s->profit_per_stock - 0.5 * s->max_drawdown;
}

questrade_algorithm_stats questrade_find_best_ma_crossover(questrade_candle *candles, int candles_count, int *slow_period_out, int *fast_period_out)
{
    int step; /* step for grid search, smaller number = longer loading times */
    int fast_min = 1, fast_max = 50;
    int slow_min = 5, slow_max = 300;
    int min_gap  = 2;

    questrade_crossover_settings settings;
    settings = questrade_get_crossover_settings();
    step = settings.step;

    /* Clamp to data length */
    if (slow_max > candles_count - 2) slow_max = candles_count - 2;
    if (fast_max > candles_count - 2) fast_max = candles_count - 2;

    slow_min = step;

    int *signals = malloc(sizeof(int) * candles_count);
    int *signal_idx = malloc(sizeof(int) * candles_count);

    questrade_algorithm_stats best_stats = (questrade_algorithm_stats){0};
    double best_score = -DBL_MAX;
    int best_fast = -1, best_slow = -1;

    for (int f = fast_min; f <= fast_max; f += step)
    {
        for (int s = slow_min; s <= slow_max; s += step)
        {
            if (f + min_gap > s) continue;

            int signals_count = questrade_calc_ma_crossover(
                candles, candles_count,
                s, f,
                signal_idx, signals, candles_count
            );

            if (signals_count <= 0) continue;

            questrade_algorithm_stats stats =
                questrade_calc_ma_crossover_profit(
                    candles, candles_count,
                    signal_idx, signals, signals_count
                );

            double sc = score_stats(&stats);
            if (sc > best_score)
            {
                best_score = sc;
                best_stats = stats;
                best_fast = f;
                best_slow = s;
            }
        }
    }

    free(signals);
    free(signal_idx);

    if (fast_period_out) *fast_period_out = best_fast;
    if (slow_period_out) *slow_period_out = best_slow;

    return best_stats;
}

static double score(const questrade_algorithm_stats *s)
{
    int trades = s->wins + s->losses;
    if (trades <= 0) return -INFINITY;

    double win_rate = (double)s->wins / (double)trades;
    double conf = log((double)trades + 1.0);

    return (s->profit_per_stock / (1 + s->max_drawdown)) * win_rate * conf;
}

int questrade_algorithm_stats_compare(const void *a, const void *b)
{
    const questrade_crossover_stats *A = a;
    const questrade_crossover_stats *B = b;

    double sa = score(&A->stats);
    double sb = score(&B->stats);

    if (sa < sb) return 1;
    if (sa > sb) return -1;
    return 0;
}

cJSON *questrade_crossover_analyze_run_tsx30_sector(questrade_TSX_30_Stock *stocks, int tsx30_count)
{
    int tsx30_stats_count = 0;
    questrade_crossover_stats all_tsx30_stats[tsx30_count];
    cJSON *json_array;

    json_array = cJSON_CreateArray();
    if (!json_array)
        return NULL;

    for (int t = 0; t < tsx30_count; t++)
    {
        questrade_candle *candles = NULL;
        int candles_count, slow_period, fast_period;
        char start_str[256], end_str[256];

        questrade_get_iso_time_str(end_str, start_str, 365);
        candles_count = questrade_fetch_candle(&candles, NULL, stocks[t].questrade_id, start_str, end_str, "OneDay");
        if (candles_count <= 0)
        {
            printf("Error fetching candles... skipping %d\n", stocks[t].questrade_id);
            continue;
        }

        all_tsx30_stats[tsx30_stats_count].stats = questrade_find_best_ma_crossover(candles, candles_count, &slow_period, &fast_period);
        all_tsx30_stats[tsx30_stats_count].fast_period = fast_period;
        all_tsx30_stats[tsx30_stats_count].slow_period = slow_period;
        strncpy(all_tsx30_stats[tsx30_stats_count].symbol, stocks[t].symbol, sizeof(all_tsx30_stats[tsx30_stats_count].symbol));
        all_tsx30_stats[tsx30_stats_count].symbolId = stocks[t].questrade_id;
        tsx30_stats_count++;
        free(candles);
    }
    qsort(all_tsx30_stats, tsx30_stats_count, sizeof(questrade_crossover_stats), questrade_algorithm_stats_compare);

    for (int i = 0; i < tsx30_stats_count; i++)
    {
        cJSON *json_obj = cJSON_CreateObject();

        cJSON_AddNumberToObject(json_obj, "rank", i + 1);
        cJSON_AddStringToObject(json_obj, "symbol", all_tsx30_stats[i].symbol);
        cJSON_AddNumberToObject(json_obj, "questrade_id", all_tsx30_stats[i].symbolId);
        cJSON_AddNumberToObject(json_obj, "fast_period", all_tsx30_stats[i].fast_period);
        cJSON_AddNumberToObject(json_obj, "slow_period", all_tsx30_stats[i].slow_period);
        cJSON_AddNumberToObject(json_obj, "profit", all_tsx30_stats[i].stats.profit_per_stock);
        cJSON_AddNumberToObject(json_obj, "wins", all_tsx30_stats[i].stats.wins);
        cJSON_AddNumberToObject(json_obj, "losses", all_tsx30_stats[i].stats.losses);

        cJSON_AddItemToArray(json_array, json_obj);
    }
    return json_array;
}

