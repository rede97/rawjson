#ifndef __CTYPE_SERILIZE_H__
#define __CTYPE_SERILIZE_H__

#include <stdint.h>

char* u32toa_branchlut2(uint32_t x, char *p);
char* i32toa_branchlut2(int32_t x, char *p);
char* u64toa_branchlut2(uint64_t x, char *p);
char* i64toa_branchlut2(int64_t x, char *p);

#endif
