#include "ctype_serialize.h"
/*==============================================================================
 * Integer Writer
 *
 * The maximum value of uint32_t is 4294967295 (10 digits),
 * these digits are named as 'aabbccddee' here.
 *
 * Although most compilers may convert the "division by constant value" into
 * "multiply and shift", manual conversion can still help some compilers
 * generate fewer and better instructions.
 *
 * Reference:
 * Division by Invariant Integers using Multiplication, 1994.
 * https://gmplib.org/~tege/divcnst-pldi94.pdf
 * Improved division by invariant integers, 2011.
 * https://gmplib.org/~tege/division-paper.pdf
 *============================================================================*/

/** Digit table from 00 to 99. */
rawjson_align(2) const char digit_table[200] = {
    '0', '0', '0', '1', '0', '2', '0', '3', '0', '4',
    '0', '5', '0', '6', '0', '7', '0', '8', '0', '9',
    '1', '0', '1', '1', '1', '2', '1', '3', '1', '4',
    '1', '5', '1', '6', '1', '7', '1', '8', '1', '9',
    '2', '0', '2', '1', '2', '2', '2', '3', '2', '4',
    '2', '5', '2', '6', '2', '7', '2', '8', '2', '9',
    '3', '0', '3', '1', '3', '2', '3', '3', '3', '4',
    '3', '5', '3', '6', '3', '7', '3', '8', '3', '9',
    '4', '0', '4', '1', '4', '2', '4', '3', '4', '4',
    '4', '5', '4', '6', '4', '7', '4', '8', '4', '9',
    '5', '0', '5', '1', '5', '2', '5', '3', '5', '4',
    '5', '5', '5', '6', '5', '7', '5', '8', '5', '9',
    '6', '0', '6', '1', '6', '2', '6', '3', '6', '4',
    '6', '5', '6', '6', '6', '7', '6', '8', '6', '9',
    '7', '0', '7', '1', '7', '2', '7', '3', '7', '4',
    '7', '5', '7', '6', '7', '7', '7', '8', '7', '9',
    '8', '0', '8', '1', '8', '2', '8', '3', '8', '4',
    '8', '5', '8', '6', '8', '7', '8', '8', '8', '9',
    '9', '0', '9', '1', '9', '2', '9', '3', '9', '4',
    '9', '5', '9', '6', '9', '7', '9', '8', '9', '9'};

char *write_u32_len_8(uint32_t val, char *buf)
{
    /* 8 digits: aabbccdd */
    uint32_t aa, bb, cc, dd, aabb, ccdd;
    aabb = (uint32_t)(((uint64_t)val * 109951163) >> 40); /* (val / 10000) */
    ccdd = val - aabb * 10000;                            /* (val % 10000) */
    aa = (aabb * 5243) >> 19;                             /* (aabb / 100) */
    cc = (ccdd * 5243) >> 19;                             /* (ccdd / 100) */
    bb = aabb - aa * 100;                                 /* (aabb % 100) */
    dd = ccdd - cc * 100;                                 /* (ccdd % 100) */
    ((v16 *)buf)[0] = ((const v16 *)digit_table)[aa];
    ((v16 *)buf)[1] = ((const v16 *)digit_table)[bb];
    ((v16 *)buf)[2] = ((const v16 *)digit_table)[cc];
    ((v16 *)buf)[3] = ((const v16 *)digit_table)[dd];
    return buf + 8;
}

char *write_u32_len_4(uint32_t val, char *buf)
{
    /* 4 digits: aabb */
    uint32_t aa, bb;
    aa = (val * 5243) >> 19; /* (val / 100) */
    bb = val - aa * 100;     /* (val % 100) */
    ((v16 *)buf)[0] = ((const v16 *)digit_table)[aa];
    ((v16 *)buf)[1] = ((const v16 *)digit_table)[bb];
    return buf + 4;
}

char *write_u32(uint32_t val, char *buf)
{
    uint32_t aa, bb, cc, dd, aabb, bbcc, ccdd, lz;

    if (val < 100)
    { /* 1-2 digits: aa */
        lz = val < 10;
        ((v16 *)buf)[0] = *(const v16 *)&(digit_table[val * 2 + lz]);
        buf -= lz;
        return buf + 2;
    }
    else if (val < 10000)
    {                            /* 3-4 digits: aabb */
        aa = (val * 5243) >> 19; /* (val / 100) */
        bb = val - aa * 100;     /* (val % 100) */
        lz = aa < 10;
        ((v16 *)buf)[0] = *(const v16 *)&(digit_table[aa * 2 + lz]);
        buf -= lz;
        ((v16 *)buf)[1] = ((const v16 *)digit_table)[bb];
        return buf + 4;
    }
    else if (val < 1000000)
    {                                                    /* 5-6 digits: aabbcc */
        aa = (uint32_t)(((uint64_t)val * 429497) >> 32); /* (val / 10000) */
        bbcc = val - aa * 10000;                         /* (val % 10000) */
        bb = (bbcc * 5243) >> 19;                        /* (bbcc / 100) */
        cc = bbcc - bb * 100;                            /* (bbcc % 100) */
        lz = aa < 10;
        ((v16 *)buf)[0] = *(const v16 *)&(digit_table[aa * 2 + lz]);
        buf -= lz;
        ((v16 *)buf)[1] = ((const v16 *)digit_table)[bb];
        ((v16 *)buf)[2] = ((const v16 *)digit_table)[cc];
        return buf + 6;
    }
    else
    {                                                         /* 7-8 digits: aabbccdd */
        aabb = (uint32_t)(((uint64_t)val * 109951163) >> 40); /* (val / 10000) */
        ccdd = val - aabb * 10000;                            /* (val % 10000) */
        aa = (aabb * 5243) >> 19;                             /* (aabb / 100) */
        cc = (ccdd * 5243) >> 19;                             /* (ccdd / 100) */
        bb = aabb - aa * 100;                                 /* (aabb % 100) */
        dd = ccdd - cc * 100;                                 /* (ccdd % 100) */
        lz = aa < 10;
        ((v16 *)buf)[0] = *(const v16 *)&(digit_table[aa * 2 + lz]);
        buf -= lz;
        ((v16 *)buf)[1] = ((const v16 *)digit_table)[bb];
        ((v16 *)buf)[2] = ((const v16 *)digit_table)[cc];
        ((v16 *)buf)[3] = ((const v16 *)digit_table)[dd];
        return buf + 8;
    }
}

char *write_u64_len_5_8(uint32_t val, char *buf)
{
    uint32_t aa, bb, cc, dd, aabb, bbcc, ccdd, lz;

    if (val < 1000000)
    {                                                    /* 5-6 digits: aabbcc */
        aa = (uint32_t)(((uint64_t)val * 429497) >> 32); /* (val / 10000) */
        bbcc = val - aa * 10000;                         /* (val % 10000) */
        bb = (bbcc * 5243) >> 19;                        /* (bbcc / 100) */
        cc = bbcc - bb * 100;                            /* (bbcc % 100) */
        lz = aa < 10;
        ((v16 *)buf)[0] = *(const v16 *)&(digit_table[aa * 2 + lz]);
        buf -= lz;
        ((v16 *)buf)[1] = ((const v16 *)digit_table)[bb];
        ((v16 *)buf)[2] = ((const v16 *)digit_table)[cc];
        return buf + 6;
    }
    else
    { /* 7-8 digits: aabbccdd */
        /* (val / 10000) */
        aabb = (uint32_t)(((uint64_t)val * 109951163) >> 40);
        ccdd = val - aabb * 10000; /* (val % 10000) */
        aa = (aabb * 5243) >> 19;  /* (aabb / 100) */
        cc = (ccdd * 5243) >> 19;  /* (ccdd / 100) */
        bb = aabb - aa * 100;      /* (aabb % 100) */
        dd = ccdd - cc * 100;      /* (ccdd % 100) */
        lz = aa < 10;
        ((v16 *)buf)[0] = *(const v16 *)&(digit_table[aa * 2 + lz]);
        buf -= lz;
        ((v16 *)buf)[1] = ((const v16 *)digit_table)[bb];
        ((v16 *)buf)[2] = ((const v16 *)digit_table)[cc];
        ((v16 *)buf)[3] = ((const v16 *)digit_table)[dd];
        return buf + 8;
    }
}

char *write_u64(uint64_t val, char *buf)
{
    uint64_t tmp, hgh;
    uint32_t mid, low;

    if (val < 100000000)
    { /* 1-8 digits */
        buf = write_u32((uint32_t)val, buf);
        return buf;
    }
    else if (val < (uint64_t)100000000 * 100000000)
    { /* 9-16 digits */
        hgh = val / 100000000;
        low = (uint32_t)(val - hgh * 100000000); /* (val % 100000000) */
        buf = write_u32((uint32_t)hgh, buf);
        buf = write_u32_len_8(low, buf);
        return buf;
    }
    else
    { /* 17-20 digits */
        tmp = val / 100000000;
        low = (uint32_t)(val - tmp * 100000000); /* (val % 100000000) */
        hgh = (uint32_t)(tmp / 10000);
        mid = (uint32_t)(tmp - hgh * 10000); /* (tmp % 10000) */
        buf = write_u64_len_5_8((uint32_t)hgh, buf);
        buf = write_u32_len_4(mid, buf);
        buf = write_u32_len_8(low, buf);
        return buf;
    }
}