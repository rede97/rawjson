#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#include <rawjson_serialize.h>
#include "ctype_serialize.h"

rawjson_align(2) const char comma[] = ",";
rawjson_align(2) const char quote[] = "\"";
rawjson_align(2) const char end[] = "\0";

rawjson_align(2) const char object_start[] = "{";
rawjson_align(2) const char object_end[] = "}";

rawjson_align(2) const char array_start[] = "[";
rawjson_align(2) const char array_end[] = "]";

// {"e":2.72,"pi":3.14}
// ---------^^
rawjson_align(2) const char comma_field_name_start[] = ",\"";
// {"e":2.72,"pi":3.14}
// -------------^^
rawjson_align(2) const char field_name_end[] = "\":";
// {"field":"value"}
// -------^^^
rawjson_align(2) const char str_field_name_end[] = "\":\"";

rawjson_align(2) const char bool_true[] = "true";
rawjson_align(2) const char bool_false[] = "false";
rawjson_align(2) const char null[] = "null";

#define static_len(x) (sizeof(x) - 1)

#define rawjson_ret_check(ret, len) \
    if (rawjson_likely(ret > 0))    \
    {                               \
        len += ret;                 \
    }                               \
    else                            \
    {                               \
        return ret;                 \
    }

#define rawjson_write(len, ser, text, len_text)           \
    {                                                     \
        ssize_t ret = ser->write_cb(ser, text, len_text); \
        rawjson_ret_check(ret, len);                              \
    }

ssize_t rawjson_ser_obj_begin(rawjson_ser_t *ser)
{
    return ser->write_cb(ser, object_start, static_len(object_start));
}

ssize_t rawjson_ser_obj_end(rawjson_ser_t *ser)
{
    return ser->write_cb(ser, object_end, static_len(object_end));
}

ssize_t rawjson_ser_close(rawjson_ser_t *ser)
{
    return ser->write_cb(ser, end, static_len(end));
}

ssize_t rawjson_ser_comma_field(rawjson_ser_t *ser, const char *field, size_t len_field)
{
    ssize_t len = 0;
    rawjson_write(len, ser, comma_field_name_start, static_len(comma_field_name_start));
    rawjson_write(len, ser, field, len_field);
    rawjson_write(len, ser, field_name_end, static_len(field_name_end));
    return len;
}

ssize_t rawjson_ser_nocomma_field(rawjson_ser_t *ser, const char *field, size_t len_field)
{
    ssize_t len = 0;
    rawjson_write(len, ser, quote, static_len(quote));
    rawjson_write(len, ser, field, len_field);
    rawjson_write(len, ser, field_name_end, static_len(field_name_end));
    return len;
}

ssize_t rawjson_ser_i32(rawjson_ser_t *ser, int32_t number)
{
    char buf[32];
    char *end = write_i32(number, buf);
    return ser->write_cb(ser, buf, (size_t)(end - buf));
}

ssize_t rawjson_ser_u32(rawjson_ser_t *ser, uint32_t number)
{
    char buf[32];
    char *end = write_u32(number, buf);
    return ser->write_cb(ser, buf, (size_t)(end - buf));
}

ssize_t rawjson_ser_i64(rawjson_ser_t *ser, int64_t number)
{
    char buf[32];
    char *end = write_i64(number, buf);
    return ser->write_cb(ser, buf, (size_t)(end - buf));
}

ssize_t rawjson_ser_u64(rawjson_ser_t *ser, uint64_t number)
{
    char buf[32];
    char *end = write_u64(number, buf);
    return ser->write_cb(ser, buf, (size_t)(end - buf));
}

ssize_t rawjson_ser_double(rawjson_ser_t *ser, double number, int ndigit)
{
    char buf[64];
    char *out = gcvt(number, ndigit, buf);
    return ser->write_cb(ser, buf, strnlen(out, sizeof(buf)));
}

ssize_t rawjson_ser_bool(rawjson_ser_t *ser, bool val)
{
    if (val)
    {
        return ser->write_cb(ser, bool_true, static_len(bool_true));
    }
    else
    {
        return ser->write_cb(ser, bool_false, static_len(bool_false));
    }
}

size_t rawjson_ser_time_digit(rawjson_ser_t *ser, uint32_t number, uint32_t n)
{
    assert(n < 16);
    char buf[32];
    char *start = &buf[n];
    char *end = write_u32(number, start);
    char *p = end - n;
    while (start > p)
    {
        start -= 1;
        *start = '0';
    }
    return ser->write_cb(ser, start, (size_t)(end - start));
}

ssize_t rawjson_ser_time_rfc3399(rawjson_ser_t *ser, const struct tm *time, __useconds_t usec)
{
    const char time_minus[] = "-";
    const char time_plus[] = "+";
    const char time_colons[] = ":";
    const char time_T[] = "T";
    const char time_Z[] = "Z";
    const char time_dot[] = ".";
    size_t len = 0, ret = 0;
    ret = ser->write_cb(ser, quote, static_len(quote));
    rawjson_ret_check(ret, len);

    do
    {
        ret = rawjson_ser_time_digit(ser, time->tm_year + 1900, 4);
        rawjson_ret_check(ret, len);
        ret = ser->write_cb(ser, time_minus, static_len(time_minus));
        rawjson_ret_check(ret, len);
        ret = rawjson_ser_time_digit(ser, time->tm_mon + 1, 2);
        rawjson_ret_check(ret, len);
        ret = ser->write_cb(ser, time_minus, static_len(time_minus));
        rawjson_ret_check(ret, len);
        ret = rawjson_ser_time_digit(ser, time->tm_mday, 2);
        rawjson_ret_check(ret, len);
        ret = ser->write_cb(ser, time_T, static_len(time_T));
        rawjson_ret_check(ret, len);
        ret = rawjson_ser_time_digit(ser, time->tm_hour, 2);
        rawjson_ret_check(ret, len);
        ret = ser->write_cb(ser, time_colons, static_len(time_colons));
        rawjson_ret_check(ret, len);
        ret = rawjson_ser_time_digit(ser, time->tm_min, 2);
        rawjson_ret_check(ret, len);
        ret = ser->write_cb(ser, time_colons, static_len(time_colons));
        rawjson_ret_check(ret, len);
        ret = rawjson_ser_time_digit(ser, time->tm_sec, 2);
        rawjson_ret_check(ret, len);
        ret = ser->write_cb(ser, time_dot, static_len(time_dot));
        rawjson_ret_check(ret, len);
        ret = rawjson_ser_time_digit(ser, usec / 1000, 3);
        rawjson_ret_check(ret, len);
        if (time->tm_gmtoff >= 0)
        {
            if (time->tm_gmtoff == 0)
            {
                ret = ser->write_cb(ser, time_Z, static_len(time_Z));
                rawjson_ret_check(ret, len);
                break;
            }
            else
            {
                ret = ser->write_cb(ser, time_plus, static_len(time_plus));
            }
        }
        else if (time->tm_gmtoff > 0)
        {
            ret = ser->write_cb(ser, time_minus, static_len(time_minus));
        }
        rawjson_ret_check(ret, len);
        size_t gmtoff_min = time->tm_gmtoff / 60;
        ret = rawjson_ser_time_digit(ser, gmtoff_min / 60, 2);
        rawjson_ret_check(ret, len);
        ret = ser->write_cb(ser, time_colons, static_len(time_colons));
        rawjson_ret_check(ret, len);
        ret = rawjson_ser_time_digit(ser, gmtoff_min % 60, 2);
        rawjson_ret_check(ret, len);
    } while (0);

    ret = ser->write_cb(ser, quote, static_len(quote));
    rawjson_ret_check(ret, len);
    return len;
}

ssize_t rawjson_ser_localtime(rawjson_ser_t *ser, const struct timeval *tv)
{
    struct tm time;
    localtime_r(&tv->tv_sec, &time);
    return rawjson_ser_time_rfc3399(ser, &time, tv->tv_usec);
}

ssize_t rawjson_ser_gmttime(rawjson_ser_t *ser, const struct timeval *tv)
{
    struct tm time;
    gmtime_r(&tv->tv_sec, &time);
    return rawjson_ser_time_rfc3399(ser, &time, tv->tv_usec);
}

ssize_t rawjson_ser_comma_field_string(rawjson_ser_t *ser, const char *field, size_t len_field, const char *value, size_t len_value)
{
    ssize_t len = 0;
    rawjson_write(len, ser, comma_field_name_start, static_len(comma_field_name_start));
    rawjson_write(len, ser, field, len_field);
    rawjson_write(len, ser, str_field_name_end, static_len(str_field_name_end));
    rawjson_write(len, ser, value, len_value);
    rawjson_write(len, ser, quote, static_len(quote));
    return len;
}

ssize_t rawjson_ser_nocomma_field_string(rawjson_ser_t *ser, const char *field, size_t len_field, const char *value, size_t len_value)
{
    ssize_t len = 0;
    rawjson_write(len, ser, quote, static_len(quote));
    rawjson_write(len, ser, field, len_field);
    rawjson_write(len, ser, str_field_name_end, static_len(str_field_name_end));
    rawjson_write(len, ser, value, len_value);
    rawjson_write(len, ser, quote, static_len(quote));
    return len;
}

ssize_t rawjson_ser_array_begin(rawjson_ser_t *ser)
{
    return ser->write_cb(ser, array_start, static_len(array_start));
}

ssize_t rawjson_ser_array_end(rawjson_ser_t *ser)
{
    return ser->write_cb(ser, array_end, static_len(array_end));
}

ssize_t rawjson_ser_array_split(rawjson_ser_t *ser)
{
    return ser->write_cb(ser, comma, static_len(comma));
}

ssize_t rawjson_ser_string(rawjson_ser_t *ser, const char *value, size_t len_value)
{
    ssize_t len = 0;
    rawjson_write(len, ser, quote, static_len(quote));
    rawjson_write(len, ser, value, len_value);
    rawjson_write(len, ser, quote, static_len(quote));
    return len;
}

ssize_t rawjson_ser_bytes(rawjson_ser_t *ser, const char *value, size_t len_value)
{
    ssize_t ret = 0, len = 0;
    ret = rawjson_ser_array_begin(ser);
    rawjson_ret_check(ret, len);
    for (size_t idx = 0; idx < len_value; idx++)
    {
        if (idx)
        {
            ret = rawjson_ser_array_split(ser);
            rawjson_ret_check(ret, len);
        }
        ret = rawjson_ser_u32(ser, value[idx]);
        rawjson_ret_check(ret, len);
    }
    ret = rawjson_ser_array_end(ser);
    rawjson_ret_check(ret, len);
    return len;
}

ssize_t rawjson_ser_null(rawjson_ser_t *ser)
{
    ssize_t len = 0;
    rawjson_write(len, ser, null, static_len(null));
    return len;
}