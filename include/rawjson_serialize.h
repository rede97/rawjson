#ifndef __RAWJSON_H__
#define __RAWJSON_H__

#include <stddef.h>
#include <stdio.h>
#include <stdint.h>

#define RAWJSON_HUMAN_USE

#ifdef RAWJSON_HUMAN_USE
typedef enum
{
    RAWJSON_COMMA_NONE,
    RAWJSON_COMMA_START,
} rawjson_comman_state_t;
#endif

typedef enum
{
    RAWJSON_ARRAY_VALUE = 0,
    RAWJSON_ARRAY_STRING,
    RAWJSON_ARRAY_OBJECT
} rawjson_array_type_t;

typedef struct rawjson_ser_s rawjson_ser_t;

typedef ssize_t (*rawjson_write_fn)(rawjson_ser_t *ser, const char *data, size_t len);

struct rawjson_ser_s
{
    rawjson_write_fn write_cb;
    void *data;
#ifdef RAWJSON_HUMAN_USE
    rawjson_comman_state_t comma;
#endif
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
ssize_t rawjson_ser_comma_field_string(rawjson_ser_t *ser, const char *field, size_t len_field, const char *value, size_t len_value);
ssize_t rawjson_ser_nocomma_field_string(rawjson_ser_t *ser, const char *field, size_t len_field, const char *value, size_t len_value);

// rawjson value serialize
ssize_t rawjson_ser_i32(rawjson_ser_t *ser, int32_t number);
ssize_t rawjson_ser_u32(rawjson_ser_t *ser, uint32_t number);
ssize_t rawjson_ser_i64(rawjson_ser_t *ser, int64_t number);
ssize_t rawjson_ser_u64(rawjson_ser_t *ser, uint64_t number);
ssize_t rawjson_ser_double(rawjson_ser_t *ser, double number, int ndigit);
ssize_t rawjson_ser_float(rawjson_ser_t *ser, float number, int ndigit);
ssize_t rawjson_ser_true(rawjson_ser_t *ser);
ssize_t rawjson_ser_false(rawjson_ser_t *ser);

// rawjson array serialize
ssize_t rawjson_ser_array_begin(rawjson_ser_t *ser, rawjson_array_type_t type);
ssize_t rawjson_ser_array_end(rawjson_ser_t *ser, rawjson_array_type_t type);
ssize_t rawjson_ser_array_split(rawjson_ser_t *ser, rawjson_array_type_t type);
ssize_t rawjson_ser_array_string(rawjson_ser_t *ser, const char *value, size_t len_value);

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

static inline int __rawjson_array_space_begin(rawjson_ser_t *ser, rawjson_array_type_t type)
{
    rawjson_ser_array_begin(ser, type);
    return 0;
}

static inline int __rawjson_array_space_end(rawjson_ser_t *ser, rawjson_array_type_t type, int idx, int len)
{
    if (idx < len)
    {
        return 1;
    }
    ser->comma = RAWJSON_COMMA_START;
    rawjson_ser_array_end(ser, type);
    return 0;
}

#define rawjson_hser_array_loop(ser, type, idx, len) for (int idx = __rawjson_array_space_begin(ser, type), __rj_is_first_element = 1, __rj_array_type = type; __rawjson_array_space_end(ser, type, idx, len); idx++)

#define rawjson_hser_array_loop_append_elem(ser)       \
    if (__rj_is_first_element)                         \
    {                                                  \
        __rj_is_first_element = 0;                     \
    }                                                  \
    else                                               \
    {                                                  \
        rawjson_ser_array_split(ser, __rj_array_type); \
    }                                                  \
    if (RAWJSON_ARRAY_OBJECT == __rj_array_type)       \
    {                                                  \
        ser->comma = RAWJSON_COMMA_NONE;               \
    }

#define rawjson_hser_array_loop_break(ser)                     \
    {                                                          \
        __rawjson_array_space_end(ser, __rj_array_type, 0, 0); \
        break;                                                 \
    }

#endif
#endif
