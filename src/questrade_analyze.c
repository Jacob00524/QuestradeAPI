#include <stdlib.h>
#include <math.h>
#include <float.h>
#include <pthread.h>

#include "questrade.h"

double questrade_calc_moving(questrade_candle *candles, int count, int period, int backwards_offset)
{
    /* candles at index = 0 is oldest */
    double ma = 0;
    int start = (count - 1) - backwards_offset;
    if (start - (period - 1) < 0) return 0;

    for (int i = start; i > start - period; i--)
        ma += candles[i].close;
    return (ma / period);
}

/*
    returned array index = 0 is most recent point
*/
int questrade_calc_moving_series(questrade_candle *candles, int count, int time_days, double **avg_out)
{
    int avg_count = count - time_days + 1;

    if (avg_count <= 0)
    {
        *avg_out = NULL;
        return 0;
    }

    *avg_out = malloc(sizeof(double) * avg_count);
    for (int i = 0; i < avg_count; i++)
        (*avg_out)[i] = questrade_calc_moving(candles, count, time_days, i);

    return avg_count;
}
