#include <time.h>

int questrade_iso_to_tm(const char *iso, struct tm *out);
int questrade_tm_to_iso(const struct tm *tm, char *out, size_t out_size);
