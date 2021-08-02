#include <assert.h>
#include <ctype.h>
#include "rawjson_deserialize.h"

rawjson_align(2) const char *RAWJSON_VALUE_REUIRE_BOOL = "bool";
rawjson_align(2) const char *RAWJSON_VALUE_REUIRE_SINT = "sint";
rawjson_align(2) const char *RAWJSON_VALUE_REUIRE_UINT = "uint";
rawjson_align(2) const char *RAWJSON_VALUE_REUIRE_ARRAY = "array";
rawjson_align(2) const char *RAWJSON_VALUE_REUIRE_STRING = "string";
rawjson_align(2) const char *RAWJSON_VALUE_REUIRE_DOUBLE = "double";
rawjson_align(2) const char *RAWJSON_VALUE_REUIRE_OBJECT = "object";

rawjson_der_status_t rawjson_fast_der_init(rawjson_der_t *der, rawjson_der_err_cb on_err, char *json_str, size_t len_str)
{
    rawjson_der_status_t ret = RAWJSON_DER_OK;
    rawjson_der_err_t err = {0};
    der->on_err = on_err;
    der->doc = yyjson_read_opts(json_str, len_str, YYJSON_READ_INSITU, NULL, &err.yyjson);
    if (der->doc)
    {
        der->curr = yyjson_doc_get_root(der->doc);
    }
    else
    {
        ret = RAWJSON_DER_YYJSON_ERR;
        if (on_err)
        {
            on_err(der, ret, &err);
        }
    }
    return ret;
}

rawjson_der_status_t rawjson_der_init(rawjson_der_t *der, rawjson_der_err_cb on_err, const char *json_str, size_t len_str)
{
    rawjson_der_status_t ret = RAWJSON_DER_OK;
    rawjson_der_err_t err = {0};
    der->on_err = on_err;
    der->doc = yyjson_read_opts((char *)json_str, len_str, 0, NULL, &err.yyjson);
    if (der->doc)
    {
        der->curr = yyjson_doc_get_root(der->doc);
    }
    else
    {
        ret = RAWJSON_DER_YYJSON_ERR;
        if (on_err)
        {
            on_err(der, ret, &err);
        }
    }
    return ret;
}

static const char *rawjson_read_num_n(const char *num_str, size_t len, uint32_t *number)
{
    uint32_t result = 0;
    size_t idx = 0;
    assert(len > 0);
    assert(len < 9);
    while (idx < len)
    {
        char c = num_str[idx];
        if (rawjson_likely(isdigit(c)))
        {
            result = result * 10 + (c - '0');
        }
        else
        {
            return NULL;
        }
        idx += 1;
    }
    *number = result;
    return &num_str[idx];
}

const char *rawjson_read_num_util(const char *num_str, size_t max_len, char stop, uint32_t *number)
{
    uint32_t result = 0;
    size_t idx = 0;
    assert(max_len > 0);
    assert(max_len < 9);
    while (idx < max_len)
    {
        char c = num_str[idx];
        if (rawjson_likely(isdigit(c)))
        {
            result = result * 10 + (c - '0');
        }
        else if (rawjson_likely(c == stop))
        {
            idx += 1;
            break;
        }
        else if (rawjson_likely(stop == '\e'))
        {
            break;
        }
        else
        {
            return NULL;
        }
        idx += 1;
    }
    *number = result;
    return &num_str[idx];
}

rawjson_der_status_t rawjson_parse_rfc3399(const char *time_str, struct tm *time, __useconds_t *usec)
{
    int32_t val = 0;

    // read year
    time_str = rawjson_read_num_util(time_str, 8, '-', &val);
    if (time_str == NULL)
    {
        return RAWJSON_DER_VALUE_ERR;
    }
    time->tm_year = val;

    // read month
    time_str = rawjson_read_num_util(time_str, 3, '-', &val);
    if (time_str == NULL)
    {
        return RAWJSON_DER_VALUE_ERR;
    }
    time->tm_mon = val;

    // read month day
    time_str = rawjson_read_num_util(time_str, 3, 'T', &val);
    if (time_str == NULL)
    {
        return RAWJSON_DER_VALUE_ERR;
    }
    time->tm_mday = val;

    // read hour
    time_str = rawjson_read_num_util(time_str, 3, ':', &val);
    if (time_str == NULL)
    {
        return RAWJSON_DER_VALUE_ERR;
    }
    time->tm_hour = val;

    // read minute
    time_str = rawjson_read_num_util(time_str, 3, ':', &val);
    if (time_str == NULL)
    {
        return RAWJSON_DER_VALUE_ERR;
    }
    time->tm_min = val;

    // read sec
    time_str = rawjson_read_num_n(time_str, 2, &val);
    if (time_str == NULL)
    {
        return RAWJSON_DER_VALUE_ERR;
    }
    time->tm_sec = val;

    if (time_str[0] == '.')
    {
        const char *usec_str = &time_str[1];
        time_str = rawjson_read_num_util(usec_str, 6, '\e', &val);
        size_t usec_len = time_str - usec_str;
        while (usec_len++ < 6)
        {
            val = val * 10;
        }
        *usec = (__useconds_t)val;
    }

    int gmtoff_sign = 1;
    switch (time_str[0])
    {
    case 'Z':
        time->tm_gmtoff = 0;
        return RAWJSON_DER_OK;
    case '-':
        gmtoff_sign = -1;
        break;
    case '+':
        break;
    default:
        return RAWJSON_DER_VALUE_ERR;
    }
    // read hour
    time_str = rawjson_read_num_util(&time_str[1], 3, ':', &val);
    if (time_str == NULL)
    {
        return RAWJSON_DER_VALUE_ERR;
    }
    time->tm_gmtoff = val * 60;

    // read min
    time_str = rawjson_read_num_n(time_str, 2, &val);
    if (time_str == NULL)
    {
        return RAWJSON_DER_VALUE_ERR;
    }
    time->tm_gmtoff = (time->tm_gmtoff + val) * 60 * gmtoff_sign;

    return RAWJSON_DER_OK;
}
