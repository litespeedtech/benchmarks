#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#include "litespeed-table.h"

enum
{
    HPACK_HUFFMAN_FLAG_ACCEPTED = 0x01,
    HPACK_HUFFMAN_FLAG_SYM = 0x02,
    HPACK_HUFFMAN_FLAG_FAIL = 0x04,
};

struct decode_status
{
    uint8_t state;
    uint8_t eos;
};


static unsigned char *
hdec_huff_dec4bits (uint8_t src_4bits, unsigned char *dst,
                                        struct decode_status *status)
{
    const struct decode_el cur_dec_code =
        decode_tables[status->state][src_4bits];
    if (cur_dec_code.flags & HPACK_HUFFMAN_FLAG_FAIL) {
        return NULL; //failed
    }
    if (cur_dec_code.flags & HPACK_HUFFMAN_FLAG_SYM)
    {
        *dst = cur_dec_code.sym;
        dst++;
    }

    status->state = cur_dec_code.state;
    status->eos = ((cur_dec_code.flags & HPACK_HUFFMAN_FLAG_ACCEPTED) != 0);
    return dst;
}


int
lshpack_dec_huff_decode_full (const unsigned char *src, int src_len,
                                            unsigned char *dst, int dst_len)
{
    const unsigned char *p_src = src;
    const unsigned char *const src_end = src + src_len;
    unsigned char *p_dst = dst;
    unsigned char *dst_end = dst + dst_len;
    struct decode_status status = { 0, 1 };

    while (p_src != src_end)
    {
        if (p_dst == dst_end)
            return -2;
        if ((p_dst = hdec_huff_dec4bits(*p_src >> 4, p_dst, &status))
                == NULL)
            return -1;
        if (p_dst == dst_end)
            return -2;
        if ((p_dst = hdec_huff_dec4bits(*p_src & 0xf, p_dst, &status))
                == NULL)
            return -1;
        ++p_src;
    }

    if (!status.eos)
        return -1;

    return p_dst - dst;
}


#define SHORTEST_CODE 5


/* The decoder is optimized for the common case.  Most of the time, we decode
 * data whose encoding is 16 bits or shorter.  This lets us use a 64 KB table
 * indexed by two bytes of input and outputs 1, 2, or 3 bytes at a time.
 *
 * In the case a longer code is encoutered, we fall back to the original
 * Huffman decoder that supports all code lengths.
 */
int
lshpack_dec_huff_decode (const unsigned char *src, int src_len,
                                            unsigned char *dst, int dst_len)
{
    unsigned char *const orig_dst = dst;
    const unsigned char *const src_end = src + src_len;
    unsigned char *const dst_end = dst + dst_len;
    uintptr_t buf;
    unsigned avail_bits, len;
    struct hdec hdec;
    uint16_t idx;
    int r;

    avail_bits = 0;
    while (1)
    {
        if (src + sizeof(buf) <= src_end)
        {
            len = (sizeof(buf) * 8 - avail_bits) >> 3;
            avail_bits += len << 3;
            switch (len)
            {
#if UINTPTR_MAX == 18446744073709551615ull
            case 8:
                buf <<= 8;
                buf |= (uintptr_t) *src++;
            case 7:
                buf <<= 8;
                buf |= (uintptr_t) *src++;
            default:
                buf <<= 48;
                buf |= (uintptr_t) *src++ << 40;
                buf |= (uintptr_t) *src++ << 32;
                buf |= (uintptr_t) *src++ << 24;
                buf |= (uintptr_t) *src++ << 16;
#else
            case 4:
                buf <<= 8;
                buf |= (uintptr_t) *src++;
            case 3:
                buf <<= 8;
                buf |= (uintptr_t) *src++;
            default:
                buf <<= 16;
#endif
                buf |= (uintptr_t) *src++ <<  8;
                buf |= (uintptr_t) *src++ <<  0;
            }
        }
        else if (src < src_end)
            do
            {
                buf <<= 8;
                buf |= (uintptr_t) *src++;
                avail_bits += 8;
            }
            while (src < src_end && avail_bits <= sizeof(buf) * 8 - 8);
        else
            break;  /* Normal case terminating condition: out of input */

        if (dst_end - dst >= (ptrdiff_t) (8 * sizeof(buf) / SHORTEST_CODE)
                                                            && avail_bits >= 16)
        {
            /* Fast path: don't check destination bounds */
            do
            {
                idx = buf >> (avail_bits - 16);
                hdec = hdecs[idx];
                dst[0] = hdec.out[0];
                dst[1] = hdec.out[1];
                dst[2] = hdec.out[2];
                dst += hdec.lens & 3;
                avail_bits -= hdec.lens >> 2;
            }
            while (avail_bits >= 16 && hdec.lens);
            if (avail_bits < 16)
                continue;
            goto slow_path;
        }
        else
            while (avail_bits >= 16)
            {
                idx = buf >> (avail_bits - 16);
                hdec = hdecs[idx];
                len = hdec.lens & 3;
                if (len && dst + len <= dst_end)
                {
                    switch (len)
                    {
                    case 3:
                        *dst++ = hdec.out[0];
                        *dst++ = hdec.out[1];
                        *dst++ = hdec.out[2];
                        break;
                    case 2:
                        *dst++ = hdec.out[0];
                        *dst++ = hdec.out[1];
                        break;
                    default:
                        *dst++ = hdec.out[0];
                        break;
                    }
                    avail_bits -= hdec.lens >> 2;
                }
                else if (dst + len > dst_end)
                    return -2;
                else
                    goto slow_path;
            }
    }

    if (avail_bits >= SHORTEST_CODE)
    {
        idx = buf << (16 - avail_bits);
        idx |= (1 << (16 - avail_bits)) - 1;    /* EOF */
        if (idx == 0xFFFF && avail_bits < 8)
            goto end;
        /* If a byte or more of input is left, this mean there is a valid
         * encoding, not just EOF.
         */
        hdec = hdecs[idx];
        len = hdec.lens & 3;
        if (len && dst + len <= dst_end)
        {
            switch (len)
            {
            case 3:
                *dst++ = hdec.out[0];
                *dst++ = hdec.out[1];
                *dst++ = hdec.out[2];
                break;
            case 2:
                *dst++ = hdec.out[0];
                *dst++ = hdec.out[1];
                break;
            default:
                *dst++ = hdec.out[0];
                break;
            }
            avail_bits -= hdec.lens >> 2;
        }
        else if (dst + len > dst_end)
            return -2;
        else
            /* This must be an invalid code, otherwise it would have fit */
            return -1;
    }

    if (avail_bits > 0)
    {
        if (((1u << avail_bits) - 1) != (buf & ((1u << avail_bits) - 1)))
            return -1;  /* Not EOF as expected */
    }

  end:
    return dst - orig_dst;

  slow_path:
    /* Find previous byte boundary and finish decoding thence. */
    while ((avail_bits & 7) && dst > orig_dst)
        avail_bits += encode_table[ *--dst ].bits;
    assert((avail_bits & 7) == 0);
    src -= avail_bits >> 3;
    r = lshpack_dec_huff_decode_full(src, src_end - src, dst, dst_end - dst);
    if (r >= 0)
        return dst - orig_dst + r;
    else
        return r;
}
