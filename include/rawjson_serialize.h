#ifndef __RAWJSON_H__
#define __RAWJSON_H__

#include <stddef.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#ifdef RAWJSON_HUMAN_USE
typedef enum
{
    RAWJSON_COMMA_NONE,
    RAWJSON_COMMA_START,
} rawjson_comman_state_t;
#endif

typedef struct rawjson_ser_s rawjson_ser_t;
typedef struct rawjson_buf_s rj_string_t;
typedef struct rawjson_buf_s rj_bytes_t;

typedef ssize_t (*rawjson_write_fn)(rawjson_ser_t *ser, const char *data, size_t len);
typedef const char *(*map_fn)(int val, size_t *len);

struct rawjson_ser_s
{
    rawjson_write_fn write_cb;
    void *data;
#ifdef RAWJSON_HUMAN_USE
    rawjson_comman_state_t comma;
#endif
};

struct rawjson_buf_s
{
    const char* data;
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

static inline ssize_t rawjson_ser_map(rawjson_ser_t *ser, map_fn map, int val)
{
    size_t _len = 0;
    const char *_val = map(val, &_len);
    return ser->write_cb(ser, _val, _len);
}

// rawjson array serialize
ssize_t rawjson_ser_array_begin(rawjson_ser_t *ser);
ssize_t rawjson_ser_array_end(rawjson_ser_t *ser);
ssize_t rawjson_ser_array_split(rawjson_ser_t *ser);

static inline ssize_t rawjson_ser_bytes(rawjson_ser_t *ser, const char* value, size_t len_value) {
    ssize_t ret = 0;
    ssize_t len = 0;
    ret = rawjson_ser_array_begin(ser);
    if(ret < 0) { return ret; } else { len += ret; }
    for(size_t idx = 0; idx < len_value; idx++) {
        if(idx) {
            ret = rawjson_ser_array_split(ser);
            if(ret < 0) { return ret; } else { len += ret; }
        }
        ret = rawjson_ser_u32(ser, value[idx]);
        if(ret < 0) { return ret; } else { len += ret; }
    }
    ret = rawjson_ser_array_end(ser);
    if(ret < 0) { return ret; } else { len += ret; }
    return len;
}

#ifdef RAWJSON_HUMAN_USE
ssize_t static inline rawjson_hser_obj_begin(rawjson_ser_t *ser)
{
    ser->comma = RAWJSON_COMMA_NONE;
    return rawjson_ser_obj_begin(ser);
}

ssize_t static inline rawjson_hser_obj_end(rawjson_ser_t *ser)
{
    ser->comma = RAWJSON_COMMA_START;
    return rawjson_ser_obj_end(ser);
}

size_t static inline rawjson_hser_filed(rawjson_ser_t *ser, const char *field, size_t len_field)
{
    if (RAWJSON_COMMA_NONE == ser->comma)
    {
        ser->comma = RAWJSON_COMMA_START;
        return rawjson_ser_nocomma_field(ser, field, len_field);
    }
    else
    {
        return rawjson_ser_comma_field(ser, field, len_field);
    }
}

size_t static inline rawjson_hser_filed_string(rawjson_ser_t *ser, const char *field, size_t len_field, const char *value, size_t len_value)
{
    if (RAWJSON_COMMA_NONE == ser->comma)
    {
        ser->comma = RAWJSON_COMMA_START;
        return rawjson_ser_nocomma_field_string(ser, field, len_field, value, len_value);
    }
    else
    {
        return rawjson_ser_comma_field_string(ser, field, len_field, value, len_value);
    }
}

static inline int __rawjson_object_space_begin(rawjson_ser_t *ser)
{
    rawjson_hser_obj_begin(ser);
    return 1;
}

static inline int __rawjson_object_space_end(rawjson_ser_t *ser)
{
    rawjson_hser_obj_end(ser);
    return 0;
}

#define rawjson_hser_object(ser) for (int __rj_obj = __rawjson_object_space_begin(ser); __rj_obj; __rj_obj = __rawjson_object_space_end(ser))

#endif
#endif
