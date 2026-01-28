#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include "questrade.h"

static pthread_mutex_t questrade_tsx_30_mutex = PTHREAD_MUTEX_INITIALIZER;

static questrade_TSX_30_Stock tsx_30_2025_technology[5] = 
{
    { 1,  "Celestica Inc.",       "CLS",  SECTOR_TECHNOLOGY, 54297544 },
    { 6,  "Propel Holdings Inc.", "PRL",  SECTOR_TECHNOLOGY, 38696401 },
    { 15, "MDA Space Ltd.",       "MDA",  SECTOR_TECHNOLOGY, 54498921 },
    { 18, "Vitalhub Corp.",       "VHI",  SECTOR_TECHNOLOGY, 28504803 },
    { 20, "Shopify Inc.",         "SHOP", SECTOR_TECHNOLOGY, 9975421 },
};

static questrade_TSX_30_Stock tsx_30_2025_mining[18] = 
{
    { 2,  "Lundin Gold Inc.",               "LUG",  SECTOR_MINING, 8359082 },
    { 5,  "Avino Silver & Gold Mines Ltd.", "ASM",  SECTOR_MINING, 8784 },
    { 10, "Almonty Industries Inc.",        "AII",  SECTOR_MINING, 63484764 },
    { 11, "New Gold Inc.",                  "NGD",  SECTOR_MINING, 28920 },
    { 12, "Kinross Gold Corporation",       "K",    SECTOR_MINING, 24914 },
    { 13, "IAMGOLD Corporation",            "IMG",  SECTOR_MINING, 22642 },
    { 14, "Torex Gold Resources Inc.",      "TXG",  SECTOR_MINING, 14310925 },
    { 19, "Alamos Gold Inc.",               "AGI",  SECTOR_MINING, 10403395 },
    { 21, "Perpetua Resources Corp.",       "PPTA", SECTOR_MINING, 34561046 },
    { 22, "Orla Mining Ltd.",               "OLA",  SECTOR_MINING, 15926080 },
    { 23, "Cameco Corporation",             "CCO",  SECTOR_MINING, 11775 },
    { 25, "China Gold International Resources Corp. Ltd.", "CGG", SECTOR_MINING, 13219 },
    { 26, "Dundee Precious Metals Inc.",    "DPM",  SECTOR_MINING, 65829691 },
    { 27, "Eldorado Gold Corporation",      "ELD",  SECTOR_MINING, 24288726 },
    { 28, "Mineros S.A.",                   "MSA",  SECTOR_MINING, 39188039 },
    { 28, "Galiano Gold Inc.",              "GAU",  SECTOR_MINING, 30409952 },
    { 29, "Skeena Resources Limited",       "SKE",  SECTOR_MINING, 36438584 },
    { 30, "Taseko Mines Limited",           "TKO",  SECTOR_MINING, 38146 }
};

static questrade_TSX_30_Stock tsx_30_2025_indsutrial_products_services[5] = 
{
    { 3,  "Hammond Power Solutions Inc.", "HPS.A", SECTOR_INDUSTRIAL_PRODUCS_SERVICES, 21379 },
    { 4,  "TerraVest Industries Inc.",    "TVK",   SECTOR_INDUSTRIAL_PRODUCS_SERVICES, 20719539 },
    { 9,  "Bombardier Inc.",              "BBD.B", SECTOR_INDUSTRIAL_PRODUCS_SERVICES, 42597421 },
    { 16, "AtkinsRéalis Group Inc",       "ATRL",  SECTOR_INDUSTRIAL_PRODUCS_SERVICES, 54711610 },
    { 17, "Bird Construction Inc.",       "BDT",   SECTOR_INDUSTRIAL_PRODUCS_SERVICES, 10207 }
};

static questrade_TSX_30_Stock tsx_30_2025_clean_technology_energy[1] = 
{
    { 7, "5N Plus Inc.", "VNP", SECTOR_CLEAN_TECHNOLOGY_ENERGY, 18986 }
};

static questrade_TSX_30_Stock tsx_30_2025_financial_services[2] = 
{
    { 8, "Galaxy Digital Holdings Ltd.",        "GLXY", SECTOR_FINANCIAL_SERVICES, 62166796 },
    { 24, "Fairfax Financial Holdings Limited", "FFH",  SECTOR_FINANCIAL_SERVICES, 18188 }
};

static questrade_TSX_30 tsx_30_2025_stocks = 
{
    5,
    tsx_30_2025_technology,
    18,
    tsx_30_2025_mining,
    5,
    tsx_30_2025_indsutrial_products_services,
    1,
    tsx_30_2025_clean_technology_energy,
    2,
    tsx_30_2025_financial_services
};

int questrade_tsx_30_get_sector(int sector, questrade_TSX_30_Stock **out)
{
    int count;
    void *sector_start;
    questrade_TSX_30_Stock *stocks;

    if (sector <= 0 || sector > SECTOR_FINANCIAL_SERVICES)
        return 0;

    pthread_mutex_lock(&questrade_tsx_30_mutex);
    sector_start = (&tsx_30_2025_stocks + ((sizeof(int) + sizeof(questrade_TSX_30_Stock*)) * (sector - 1)));
    count = *((int*)sector_start);
    stocks = *((questrade_TSX_30_Stock**)(sector_start + sizeof(int)));

    if (count == 0 || !stocks)
    {
        pthread_mutex_unlock(&questrade_tsx_30_mutex);
        return 0;
    }

    if (out)
    {
        *out = malloc(sizeof(questrade_TSX_30_Stock) * count);
        memcpy(*out, stocks, sizeof(questrade_TSX_30_Stock) * count);
    }

    pthread_mutex_unlock(&questrade_tsx_30_mutex);
    return count;
}

int questrade_tsx_30_get_all(questrade_TSX_30_Stock **out)
{
    int count;

    pthread_mutex_lock(&questrade_tsx_30_mutex);
    count = tsx_30_2025_stocks.technology_count + tsx_30_2025_stocks.mining_count + tsx_30_2025_stocks.indsutrial_products_services_count + tsx_30_2025_stocks.clean_technology_energy_count + tsx_30_2025_stocks.financial_services_count;

    if (out)
    {
        *out = malloc(sizeof(questrade_TSX_30_Stock) * count);
        memcpy(*out, tsx_30_2025_stocks.technology, sizeof(questrade_TSX_30_Stock) * tsx_30_2025_stocks.technology_count);
        memcpy(*out + tsx_30_2025_stocks.technology_count, tsx_30_2025_stocks.mining, sizeof(questrade_TSX_30_Stock) * tsx_30_2025_stocks.mining_count);
        memcpy(*out + tsx_30_2025_stocks.mining_count + tsx_30_2025_stocks.technology_count, tsx_30_2025_stocks.indsutrial_products_services, sizeof(questrade_TSX_30_Stock) * tsx_30_2025_stocks.indsutrial_products_services_count);
        memcpy(*out + tsx_30_2025_stocks.mining_count + tsx_30_2025_stocks.technology_count + tsx_30_2025_stocks.indsutrial_products_services_count, tsx_30_2025_stocks.clean_technology_energy, sizeof(questrade_TSX_30_Stock) * tsx_30_2025_stocks.clean_technology_energy_count);
        memcpy(*out + tsx_30_2025_stocks.mining_count + tsx_30_2025_stocks.technology_count + tsx_30_2025_stocks.indsutrial_products_services_count + tsx_30_2025_stocks.clean_technology_energy_count, tsx_30_2025_stocks.financial_services, sizeof(questrade_TSX_30_Stock) * tsx_30_2025_stocks.financial_services_count);
    }

    pthread_mutex_unlock(&questrade_tsx_30_mutex);
    return count;
}

void questrade_tsx_30_cleanup()
{
    pthread_mutex_destroy(&questrade_tsx_30_mutex);
}
