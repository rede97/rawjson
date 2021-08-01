#ifndef __CTYPE_SERILIZE_H__
#define __CTYPE_SERILIZE_H__

#include "rawjson.h"
#include "rawjson_micro.h"

/** 16/32/64-bit vector */
typedef struct v16 { char c1, c2; } v16;
typedef struct v32 { char c1, c2, c3, c4; } v32;
typedef struct v64 { char c1, c2, c3, c4, c5, c6, c7, c8; } v64;

char *write_u32(uint32_t val, char *buf);
char *write_u64(uint64_t val, char *buf);

static rawjson_inline char *write_i64(int64_t val, char *buf)
{
    uint64_t t;
    if (val >= 0)
        t = val;
    else
        *buf++ = '-', t = -(uint64_t)val;
    return write_u64(t, buf);
}

static rawjson_inline char *write_i32(int32_t val, char *buf)
{
    uint64_t t;
    if (val >= 0)
        t = val;
    else
        *buf++ = '-', t = -(uint32_t)val;
    return write_u32(t, buf);
}

#endif
