#ifndef __RAWJSON_DESERIALIZE_H__
#define __RAWJSON_DESERIALIZE_H__

#include "../yyjson/src/yyjson.h"
#include "rawjson.h"

typedef enum
{
    RAWJSON_DER_OK,
    RAWJSON_DER_YYJSON_ERR,
    RAWJSON_DER_FIELD_ERR,
    RAWJSON_DER_VALUE_ERR,
} rawjson_der_status_t;

typedef enum
{
    RAWJSON_FIELD_UNKNOWN,
    RAWJSON_FIELD_MISS
} rawjson_der_field_err_t;

typedef enum
{
    RAWJSON_VALUE_NULL,
    RAWJSON_VALUE_INVAILD_TYPE,
    RAWJSON_VALUE_IVAILD_FORMAT,
    RAWJSON_VALUE_OUT_OF_RANGE,
} rawjson_der_value_err_t;

typedef struct rawjson_field_err_s
{
    rawjson_der_field_err_t code;
    const char *msg;
} rawjson_field_err_t;

typedef struct rawjson_value_err_s
{
    rawjson_der_value_err_t code;
    const char *msg;
    const char *value;
} rawjson_value_err_t;

typedef union rawjson_der_err_u
{
    yyjson_read_err yyjson;
    rawjson_field_err_t field;
    rawjson_value_err_t value;
} rawjson_der_err_t;

typedef struct rawjson_der_s rawjson_der_t;
typedef void (*rawjson_der_err_cb)(rawjson_der_t *der, rawjson_der_status_t type, const rawjson_der_err_t *err);

struct rawjson_der_s
{
    yyjson_doc *doc;
    yyjson_val *curr;
    rawjson_der_err_cb on_err;
    void *data;
};

#endif