#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>

#include <unistd.h>

#include <sys/time.h>

#include "ctype_serialize.h"

#define RAWJSON_HUMAN_USE
#include "rawjson_serialize.h"

typedef char *(*itoa_fn)(int, char *);
typedef char *(*dtoa_fn)(double, char *);

static char *itoa_std(int integer, char *out)
{
    int n = sprintf(out, "%d", integer);
    return &out[n];
}

static char *itoa_branchlut(int integer, char *out)
{
    return i32toa_branchlut2(integer, out);
}

void test_i32_conv(int32_t *arr, size_t n, itoa_fn fn)
{
    char buffer[32];
    for (size_t i = 0; i < n; i++)
    {
        fn(arr[i], buffer);
    }
}

int test_integer()
{
    int array[1000000] = {0};
    size_t array_len = sizeof(array) / sizeof(array[0]);
    for (size_t i = 0; i < array_len; i++)
    {
        array[i] = rand();
        {
            char buf_std[32];
            char buf_branchlut[32];
            itoa_std(array[i], buf_std);
            itoa_branchlut(array[i], buf_branchlut);
            if (strcmp(buf_std, buf_branchlut))
            {
                printf("error: [%ld] => %s != %s\n", i, buf_std, buf_branchlut);
                return -1;
            }
        }
    }

    struct timeval time_start = {0, 0}, time_end = {0, 0};
    gettimeofday(&time_start, NULL);
    test_i32_conv(array, array_len, itoa_std);
    gettimeofday(&time_end, NULL);
    int64_t duration_std = (time_end.tv_sec - time_start.tv_sec) * 1e9 + (time_end.tv_usec - time_start.tv_usec);

    gettimeofday(&time_start, NULL);
    test_i32_conv(array, array_len, itoa_branchlut);
    gettimeofday(&time_end, NULL);
    int64_t duration_branchlut = (time_end.tv_sec - time_start.tv_sec) * 1e9 + (time_end.tv_usec - time_start.tv_usec);
    printf("[itoa] branchlut fast than std: %.2lfx\n", duration_std / (double)duration_branchlut);
}

typedef struct
{
    rawjson_ser_t ser;
    int fd;
    size_t cnt;
} file_json_ser_t;

ssize_t file_json_ser_write(rawjson_ser_t *ser, const char *data, size_t len)
{
    file_json_ser_t *file_ser = (file_json_ser_t *)ser;
    ssize_t ret = write(file_ser->fd, data, len);
    if (ret > 0)
    {
        file_ser->cnt += ret;
    }
    return ret;
}

void test_json_serialize()
{
    file_json_ser_t file_ser = {
        .ser = {
            .write_cb = file_json_ser_write,
        },
        .fd = STDOUT_FILENO,
    };
    rawjson_ser_t *ser = (rawjson_ser_t *)&file_ser;

    {
        rawjson_hser_object(ser)
        {
            rawjson_hser_filed(ser, "obj", 4);
            rawjson_hser_object(ser)
            {
                rawjson_hser_filed_string(ser, "Hello", 5, "World", 5);
                rawjson_hser_filed(ser, "new", 3);
                rawjson_ser_float(ser, 123.456, 5);
            }
            rawjson_hser_filed(ser, "arr", 3);
            rawjson_hser_array_loop(ser, i, 10)
            {
                if (7 == i)
                    rawjson_hser_array_loop_break(ser);
                rawjson_hser_array_loop_append_elem(ser);
                rawjson_hser_object(ser)
                {
                    rawjson_hser_filed(ser, "idx", 3);
                    rawjson_ser_i32(ser, i);
                    rawjson_hser_filed(ser, "val", 3);
                    rawjson_ser_float(ser, sinf(i / 8.0 * M_PI), 5);
                    rawjson_hser_filed(ser, "subarray", 8);
                    rawjson_hser_array_loop(ser, j, 5)
                    {
                        const char *test_str[] = {"Hello", "World", "RawJson", "Protobuf", "Rust"};
                        if (2 == j)
                            rawjson_hser_array_loop_break(ser);

                        rawjson_hser_array_loop_append_elem(ser);
                        size_t len = strlen(test_str[j]);
                        rawjson_ser_array_string(ser, test_str[j], len);
                    }
                    rawjson_hser_filed(ser, "zero", 4);
                    rawjson_ser_true(ser);
                }
            }
        }
    }
    printf("\njson size: %ld\n", file_ser.cnt);
}

int main()
{
    test_integer();
    test_json_serialize();
}