#include <rawjson_serialize.h>
#include <string.h>
#include <stdlib.h>

#include "ctype_serialize.h"

const char comma[] = ",";
const char quote[] = "\"";
const char end[] = "\0";

const char object_start[] = "{";
const char object_end[] = "}";

const char array_start[] = "[";
const char array_end[] = "]";

// {"e":2.72,"pi":3.14}
// ---------^^
const char comma_field_name_start[] = ",\"";
// {"e":2.72,"pi":3.14}
// -------------^^
const char field_name_end[] = "\":";
// {"field":"value"}
// -------^^^
const char str_field_name_end[] = "\":\"";

const char bool_true[] = "true";
const char bool_false[] = "false";

#define static_len(x) (sizeof(x) - 1)

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

ssize_t rawjson_ser_double(rawjson_ser_t *ser, double number, int ndigit)
{
    char buf[64];
    char *out = gcvt(number, ndigit, buf);
    return ser->write_cb(ser, buf, strnlen(out, sizeof(buf)));
}

ssize_t rawjson_ser_true(rawjson_ser_t *ser)
{
    return ser->write_cb(ser, bool_true, static_len(bool_true));
}

ssize_t rawjson_ser_false(rawjson_ser_t *ser)
{
    return ser->write_cb(ser, bool_false, static_len(bool_false));
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

ssize_t rawjson_ser_array_string(rawjson_ser_t *ser, const char *value, size_t len_value)
{
    ssize_t len = 0;
    rawjson_write(len, ser, quote, static_len(quote));
    rawjson_write(len, ser, value, len_value);
    rawjson_write(len, ser, quote, static_len(quote));
    return len;
}
