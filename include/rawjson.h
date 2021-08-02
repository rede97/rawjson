#ifndef __RAWJSON_H__
#define __RAWJSON_H__

#ifndef __USE_MISC
#define __USE_MISC
#endif

#include <stddef.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <time.h>
#include <sys/time.h>

typedef struct rawjson_buf_s rj_string_t;
typedef struct rawjson_buf_s rj_bytes_t;

struct rawjson_buf_s
{
    const char *data;
    size_t len;
};

#endif