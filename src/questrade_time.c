#include <stdio.h>
#include <string.h>
#include <time.h>

int questrade_iso_to_tm(const char *iso, struct tm *out)
{
    if (!iso || !out) return 0;

    int Y, M, D, h, m, s;

    int n = sscanf(iso, "%d-%d-%dT%d:%d:%d",
                   &Y, &M, &D, &h, &m, &s);

    if (n != 6) return 0;

    memset(out, 0, sizeof(*out));
    out->tm_year = Y - 1900;
    out->tm_mon  = M - 1;
    out->tm_mday = D;
    out->tm_hour = h;
    out->tm_min  = m;
    out->tm_sec  = s;
    out->tm_isdst = -1;

    return 1;
}
int questrade_tm_to_iso(const struct tm *tm, char *out, size_t out_size)
{
    char base[32];
    struct tm tmp = *tm;

    if (!tm || !out || out_size == 0)
        return 0;

    if (mktime(&tmp) == -1)
        return 0;
    if (!strftime(base, sizeof(base), "%Y-%m-%dT%H:%M:%S", &tmp))
        return 0;

    snprintf(out, out_size, "%s.000000-05:00", base);
    return 1;
}
