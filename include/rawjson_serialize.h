#ifndef __RAWJSON_H__
#define __RAWJSON_H__

#include <stddef.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <sys/time.h>

typedef enum
{
    RAWJSON_COMMA_NONE,
    RAWJSON_COMMA_START,
} rawjson_comman_state_t;

typedef struct rawjson_ser_s rawjson_ser_t;
typedef struct rawjson_buf_s rj_string_t;
typedef struct rawjson_buf_s rj_bytes_t;

typedef ssize_t (*rawjson_write_fn)(rawjson_ser_t *ser, const char *data, size_t len);
typedef const char *(*map_fn)(int val, size_t *len);

struct rawjson_ser_s
{
    rawjson_write_fn write_cb;
    void *data;
};

struct rawjson_buf_s
{
    const char *data;
    size_t len;
};

// rawjson object serialize
ssize_t rawjson_ser_obj_begin(rawjson_ser_t *ser);
ssize_t rawjson_ser_obj_end(rawjson_ser_t *ser);

// rawjson close
ssize_t rawjson_ser_close(rawjson_ser_t *ser);

// rawjson field serialize
ssize_t rawjson_ser_comma_field(rawjson_ser_t *ser, const char *field, size_t len_field);
ssize_t rawjson_ser_nocomma_field(rawjson_ser_t *ser, const char *field, size_t len_field);

// rawjson string type field serialize
ssize_t rawjson_ser_string(rawjson_ser_t *ser, const char *value, size_t len_value);
ssize_t rawjson_ser_comma_field_string(rawjson_ser_t *ser, const char *field, size_t len_field, const char *value, size_t len_value);
ssize_t rawjson_ser_nocomma_field_string(rawjson_ser_t *ser, const char *field, size_t len_field, const char *value, size_t len_value);

// rawjson value serialize
ssize_t rawjson_ser_i32(rawjson_ser_t *ser, int32_t number);
ssize_t rawjson_ser_u32(rawjson_ser_t *ser, uint32_t number);
ssize_t rawjson_ser_i64(rawjson_ser_t *ser, int64_t number);
ssize_t rawjson_ser_u64(rawjson_ser_t *ser, uint64_t number);
ssize_t rawjson_ser_double(rawjson_ser_t *ser, double number, int ndigit);

static ssize_t rawjson_ser_float(rawjson_ser_t *ser, float number, int ndigit)
{
    return rawjson_ser_double(ser, (double)number, ndigit);
}

ssize_t rawjson_ser_bool(rawjson_ser_t *ser, bool val);

ssize_t rawjson_ser_null(rawjson_ser_t *ser);

// rawjson mapfn serialize
static inline ssize_t rawjson_ser_map(rawjson_ser_t *ser, map_fn map, int val)
{
    size_t _len = 0;
    const char *_val = map(val, &_len);
    return ser->write_cb(ser, _val, _len);
}

// rawjson time serialize
ssize_t rawjson_ser_localtime(rawjson_ser_t *ser, const struct timeval *tv);
ssize_t rawjson_ser_gmttime(rawjson_ser_t *ser, const struct timeval *tv);

// rawjson array serialize
ssize_t rawjson_ser_array_begin(rawjson_ser_t *ser);
ssize_t rawjson_ser_array_end(rawjson_ser_t *ser);
ssize_t rawjson_ser_array_split(rawjson_ser_t *ser);

// rawjson bytes serialize
ssize_t rawjson_ser_bytes(rawjson_ser_t *ser, const char *value, size_t len_value);

ssize_t static inline rawjson_hser_obj_begin(rawjson_comman_state_t *comma, rawjson_ser_t *ser)
{
    *comma = RAWJSON_COMMA_NONE;
    return rawjson_ser_obj_begin(ser);
}

ssize_t static inline rawjson_hser_obj_end(rawjson_comman_state_t *comma, rawjson_ser_t *ser)
{
    *comma = RAWJSON_COMMA_START;
    return rawjson_ser_obj_end(ser);
}

size_t static inline rawjson_hser_filed(rawjson_comman_state_t *comma, rawjson_ser_t *ser, const char *field, size_t len_field)
{
    if (RAWJSON_COMMA_NONE == *comma)
    {
        *comma = RAWJSON_COMMA_START;
        return rawjson_ser_nocomma_field(ser, field, len_field);
    }
    else
    {
        return rawjson_ser_comma_field(ser, field, len_field);
    }
}

size_t static inline rawjson_hser_filed_string(rawjson_comman_state_t *comma, rawjson_ser_t *ser, const char *field, size_t len_field, const char *value, size_t len_value)
{
    if (RAWJSON_COMMA_NONE == *comma)
    {
        *comma = RAWJSON_COMMA_START;
        return rawjson_ser_nocomma_field_string(ser, field, len_field, value, len_value);
    }
    else
    {
        return rawjson_ser_comma_field_string(ser, field, len_field, value, len_value);
    }
}

#endif
