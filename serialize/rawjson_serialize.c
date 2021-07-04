#include <rawjson_serialize.h>
#include <string.h>
#include <stdlib.h>

#include "ctype_serialize.h"

const char comma[] = ",";
const char quote[] = "\"";
const char end[] = "\0";

const char object_start[] = "{\"";
const char object_end[] = "}";

const char array_start_value[] = "[";
const char array_split_value[] = ",";
const char array_end_value[] = "]";

const char array_start_string[] = "[\"";
const char array_split_string[] = "\",\"";
const char array_end_string[] = "\"]";

const char array_start_object[] = "[{\"";
const char array_split_object[] = "},{\"";
const char array_end_object[] = "}]";

// {"e":2.72,"pi":3.14}
// ---------^^
const char comma_field_name_start[] = ",\"";
// {"e":2.72,"pi":3.14}
// -------------^^
const char field_name_end[] = "\":";
// {"field":"value"}
// -------^^^
const char str_field_name_end[] = "\":\"";

#define static_len(x) (sizeof(x) - 1)

#define rawjson_write(len, ret, ser, text, len_text) \
                                                     \
    {                                                \
        ret = ser->write_cb(ser, text, len_text);    \
        if (ret != len_text)                         \
        {                                            \
            return ret;                              \
        }                                            \
        len += ret;                                  \
    }

ssize_t inline rawjson_ser_obj_begin(rawjson_ser_t *ser)
{
    return ser->write_cb(ser, object_start, static_len(object_start));
}

ssize_t inline rawjson_ser_obj_end(rawjson_ser_t *ser)
{
    return ser->write_cb(ser, object_end, static_len(object_end));
}

ssize_t inline rawjson_ser_close(rawjson_ser_t *ser)
{
    return ser->write_cb(ser, end, static_len(end));
}

ssize_t inline rawjson_ser_comma_field(rawjson_ser_t *ser, const char *field, size_t len_field)
{
    ssize_t len = 0;
    ssize_t ret = 0;
    rawjson_write(len, ret, ser, comma_field_name_start, static_len(comma_field_name_start));
    rawjson_write(len, ret, ser, field, len_field);
    rawjson_write(len, ret, ser, field_name_end, static_len(field_name_end));
    return len;
}

ssize_t inline rawjson_ser_nocomma_field(rawjson_ser_t *ser, const char *field, size_t len_field)
{
    ssize_t len = 0;
    ssize_t ret = 0;
    rawjson_write(len, ret, ser, field, len_field);
    rawjson_write(len, ret, ser, field_name_end, static_len(field_name_end));
    return len;
}

ssize_t rawjson_ser_i32(rawjson_ser_t *ser, int32_t number)
{
    char buf[32];
    char *end = i32toa_branchlut2(number, buf);
    return ser->write_cb(ser, buf, (size_t)(end - buf));
}

ssize_t rawjson_ser_u32(rawjson_ser_t *ser, uint32_t number)
{
    char buf[32];
    char *end = u32toa_branchlut2(number, buf);
    return ser->write_cb(ser, buf, (size_t)(end - buf));
}

ssize_t rawjson_ser_i64(rawjson_ser_t *ser, int64_t number)
{
    char buf[32];
    char *end = i64toa_branchlut2(number, buf);
    return ser->write_cb(ser, buf, (size_t)(end - buf));
}

ssize_t rawjson_ser_u64(rawjson_ser_t *ser, uint64_t number)
{
    char buf[32];
    char *end = u64toa_branchlut2(number, buf);
    return ser->write_cb(ser, buf, (size_t)(end - buf));
}

ssize_t inline rawjson_ser_double(rawjson_ser_t *ser, double number, int ndigit)
{
    char buf[64];
    char *out = gcvt(number, ndigit, buf);
    return ser->write_cb(ser, buf, strnlen(out, sizeof(buf)));
}

ssize_t rawjson_ser_float(rawjson_ser_t *ser, float number, int ndigit)
{
    return rawjson_ser_double(ser, (double)number, ndigit);
}

ssize_t rawjson_ser_true(rawjson_ser_t *ser)
{
    const char s_bool[] = "true";
    return ser->write_cb(ser, s_bool, sizeof(s_bool) - 1);
}

ssize_t rawjson_ser_false(rawjson_ser_t *ser)
{
    const char s_bool[] = "false";
    return ser->write_cb(ser, s_bool, sizeof(s_bool) - 1);
}

ssize_t rawjson_ser_comma_field_string(rawjson_ser_t *ser, const char *field, size_t len_field, const char *value, size_t len_value)
{
    ssize_t len = 0;
    ssize_t ret = 0;
    rawjson_write(len, ret, ser, comma_field_name_start, static_len(comma_field_name_start));
    rawjson_write(len, ret, ser, field, len_field);
    rawjson_write(len, ret, ser, str_field_name_end, static_len(str_field_name_end));
    rawjson_write(len, ret, ser, value, len_value);
    rawjson_write(len, ret, ser, quote, static_len(quote));
    return len;
}

ssize_t rawjson_ser_nocomma_field_string(rawjson_ser_t *ser, const char *field, size_t len_field, const char *value, size_t len_value)
{
    ssize_t len = 0;
    ssize_t ret = 0;
    size_t len_text = strnlen(field, len_field);
    rawjson_write(len, ret, ser, field, len_text);
    rawjson_write(len, ret, ser, str_field_name_end, static_len(str_field_name_end));
    rawjson_write(len, ret, ser, value, len_value);
    rawjson_write(len, ret, ser, quote, static_len(quote));
    return len;
}

ssize_t inline rawjson_ser_array_begin(rawjson_ser_t *ser, rawjson_array_type_t type)
{
    const char *array_start = NULL;
    size_t len_array_start = 0;
    switch (type)
    {
    case RAWJSON_ARRAY_VALUE:
        array_start = array_start_value;
        len_array_start = static_len(array_start_value);
        break;
    case RAWJSON_ARRAY_STRING:
        array_start = array_start_string;
        len_array_start = static_len(array_start_string);
        break;
    case RAWJSON_ARRAY_OBJECT:
        array_start = array_start_object;
        len_array_start = static_len(array_start_object);
        break;
    default:
        return 0;
    }
    return ser->write_cb(ser, array_start, len_array_start);
}

ssize_t inline rawjson_ser_array_end(rawjson_ser_t *ser, rawjson_array_type_t type)
{
    const char *array_end = NULL;
    size_t len_array_end = 0;
    switch (type)
    {
    case RAWJSON_ARRAY_VALUE:
        array_end = array_end_value;
        len_array_end = static_len(array_end_value);
        break;
    case RAWJSON_ARRAY_STRING:
        array_end = array_end_string;
        len_array_end = static_len(array_end_string);
        break;
    case RAWJSON_ARRAY_OBJECT:
        array_end = array_end_object;
        len_array_end = static_len(array_end_object);
        break;
    default:
        return 0;
    }
    return ser->write_cb(ser, array_end, len_array_end);
}

ssize_t inline rawjson_ser_array_split(rawjson_ser_t *ser, rawjson_array_type_t type)
{
    const char *array_split = NULL;
    size_t len_array_split = 0;
    switch (type)
    {
    case RAWJSON_ARRAY_VALUE:
        array_split = array_split_value;
        len_array_split = static_len(array_split_value);
        break;
    case RAWJSON_ARRAY_STRING:
        array_split = array_split_string;
        len_array_split = static_len(array_split_string);
        break;
    case RAWJSON_ARRAY_OBJECT:
        array_split = array_split_object;
        len_array_split = static_len(array_split_object);
        break;
    default:
        return 0;
    }
    return ser->write_cb(ser, array_split, len_array_split);
}

ssize_t inline rawjson_ser_array_string(rawjson_ser_t *ser, const char *value, size_t len_value)
{
    return ser->write_cb(ser, value, len_value);
}