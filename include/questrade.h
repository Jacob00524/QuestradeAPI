#define SECTOR_UNKNOWN 0
#define SECTOR_TECHNOLOGY 1
#define SECTOR_MINING 2
#define SECTOR_INDUSTRIAL_PRODUCS_SERVICES 3
#define SECTOR_CLEAN_TECHNOLOGY_ENERGY 4
#define SECTOR_FINANCIAL_SERVICES 5

#include "../external/cJSON/cJSON.h"
#include "questrade_structs.h"
#include "questrade_globals.h"
#include "questrade_parse.h"
#include "questrade_login.h"
#include "questrade_fetch.h"
#include "questrade_analyze.h"
#include "questrade_time.h"
#include "questrade_algorithms.h"

int questrade_tsx_30_get_sector(int sector, questrade_TSX_30_Stock **out);
int questrade_tsx_30_get_all(questrade_TSX_30_Stock **out);
void questrade_tsx_30_cleanup();

int questrade_init(char *consumer_key, char *callback);
void questrade_cleanup();
