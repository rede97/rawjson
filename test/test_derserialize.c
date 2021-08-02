#include "rawjson_deserialize.h"

rawjson_der_status_t rawjson_parse_rfc3399(const char *time_str, struct tm *time, __useconds_t *usec);

int test_parse_rfc3399()
{
    const char *rfc3399[] = {
        "2021-08-03T00:44:53+08:00",
        "2021-08-03T00:44:53.254+08:00",
        "2021-08-03T00:44:53.254123+08:00",
        "2021-08-03T00:44:53Z",
        "2021-08-03T00:44:53.254Z",
        "2021-08-03T00:44:53.254-08:00",
    };
    const struct tm expect_time = {
        .tm_year = 2021,
        .tm_mon = 8,
        .tm_mday = 3,
        .tm_hour = 0,
        .tm_min = 44,
        .tm_sec = 53,
    };
    const int expect_gmtoff[] = {
        8 * 3600,
        8 * 3600,
        8 * 3600,
        0,
        0,
        -8 * 3600,
    };
    const __useconds_t expect_usec[] = {0, 254000, 254123, 0, 254000, 254000, 254000};
    for (int i = 0; i < sizeof(rfc3399) / sizeof(rfc3399[0]); i++)
    {
        struct tm parsed_time = {0};
        __useconds_t parsed_usec = 0;
        if (RAWJSON_DER_OK != rawjson_parse_rfc3399(rfc3399[i], &parsed_time, &parsed_usec))
        {
            return -1;
        }
        if (parsed_time.tm_year != expect_time.tm_year ||
            parsed_time.tm_mon != expect_time.tm_mon ||
            parsed_time.tm_mday != expect_time.tm_mday ||
            parsed_time.tm_hour != expect_time.tm_hour ||
            parsed_time.tm_min != expect_time.tm_min ||
            parsed_time.tm_sec != expect_time.tm_sec ||
            parsed_time.tm_gmtoff != expect_gmtoff[i] ||
            parsed_usec != expect_usec[i])
        {
            printf("parse rfc3399 faield, %d#%s\n", i, rfc3399[i]);
            const char *time_fmt = "%04d-%02d-%02dT%02d:%02d:%02d.%d #%d\n";
            printf(time_fmt, expect_time.tm_year, expect_time.tm_mon, expect_time.tm_mday, expect_time.tm_hour, expect_time.tm_min, expect_time.tm_sec, expect_usec[i], expect_gmtoff[i]);
            printf(time_fmt, parsed_time.tm_year, parsed_time.tm_mon, parsed_time.tm_mday, parsed_time.tm_hour, parsed_time.tm_min, parsed_time.tm_sec, parsed_usec, parsed_time.tm_gmtoff);
            return -2;
        }
    }

    return 0;
}

int main(int argc, const char *argv[])
{
    if (argc < 2)
    {
        puts("usage: ./test_derserialize test-item");
    }
    if (strncmp(argv[0], "rfc3399", 7))
    {
        return test_parse_rfc3399();
    }
    return 0;
}