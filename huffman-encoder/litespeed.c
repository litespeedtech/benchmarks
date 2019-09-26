#include <stdint.h>
#include <string.h>

#include "litespeed-table.h"


/* Don't use big table */
int
lshpack_enc_huff_encode_orig (const unsigned char *src,
    int src_len, unsigned char *const dst, int dst_len)
{
    unsigned char *p_dst = dst;
    const unsigned char *src_end = src + src_len;
    unsigned char *dst_end = p_dst + dst_len;
    uintptr_t bits;  /* OK not to initialize this variable */
    unsigned bits_used = 0, adj;
    struct encode_el cur_enc_code;

    while (src != src_end)
    {
        cur_enc_code = encode_table[*src++];
        if (bits_used + cur_enc_code.bits < sizeof(bits) * 8)
        {
            bits <<= cur_enc_code.bits;
            bits |= cur_enc_code.code;
            bits_used += cur_enc_code.bits;
            continue;
        }
        else if (p_dst + sizeof(bits) <= dst_end)
        {
            bits <<= sizeof(bits) * 8 - bits_used;
            bits_used = cur_enc_code.bits - (sizeof(bits) * 8 - bits_used);
            bits |= cur_enc_code.code >> bits_used;
#if UINTPTR_MAX == 18446744073709551615ull
            *p_dst++ = bits >> 56;
            *p_dst++ = bits >> 48;
            *p_dst++ = bits >> 40;
            *p_dst++ = bits >> 32;
#endif
            *p_dst++ = bits >> 24;
            *p_dst++ = bits >> 16;
            *p_dst++ = bits >> 8;
            *p_dst++ = bits;
            bits = cur_enc_code.code;   /* OK not to clear high bits */
        }
        else
            return -1;
    }

    adj = bits_used + (-bits_used & 7);     /* Round up to 8 */
    if (bits_used && p_dst + (adj >> 3) <= dst_end)
    {
        bits <<= -bits_used & 7;            /* Align to byte boundary */
        bits |= ((1 << (-bits_used & 7)) - 1);  /* EOF */
        switch (adj >> 3)
        {                               /* Write out */
#if UINTPTR_MAX == 18446744073709551615ull
        case 8: *p_dst++ = bits >> 56;
        case 7: *p_dst++ = bits >> 48;
        case 6: *p_dst++ = bits >> 40;
        case 5: *p_dst++ = bits >> 32;
#endif
        case 4: *p_dst++ = bits >> 24;
        case 3: *p_dst++ = bits >> 16;
        case 2: *p_dst++ = bits >> 8;
        default: *p_dst++ = bits;
        }
        return p_dst - dst;
    }
    else if (p_dst + (adj >> 3) <= dst_end)
        return p_dst - dst;
    else
        return -1;
}


int
lshpack_enc_huff_encode (const unsigned char *src,
    int src_len, unsigned char *const dst, int dst_len)
{
    unsigned char *p_dst = dst;
    const unsigned char *src_end = src + src_len;
    unsigned char *dst_end = p_dst + dst_len;
    uintptr_t bits;  /* OK not to initialize this variable */
    unsigned bits_used = 0, adj;
    struct encode_el cur_enc_code;
    const struct henc *henc;
    uint16_t idx;

    while (src + sizeof(bits) * 8 / 5 < src_end
                                    && p_dst + sizeof(bits) <= dst_end)
    {
        memcpy(&idx, src, 2);
        henc = &hencs[idx];
        src += 2;
        while (bits_used + henc->lens < sizeof(bits) * 8)
        {
            bits <<= henc->lens;
            bits |= henc->code;
            bits_used += henc->lens;
            memcpy(&idx, src, 2);
            henc = &hencs[idx];
            src += 2;
        }
        if (henc->lens < 64)
        {
            bits <<= sizeof(bits) * 8 - bits_used;
            bits_used = henc->lens - (sizeof(bits) * 8 - bits_used);
            bits |= henc->code >> bits_used;
#if UINTPTR_MAX == 18446744073709551615ull
            *p_dst++ = bits >> 56;
            *p_dst++ = bits >> 48;
            *p_dst++ = bits >> 40;
            *p_dst++ = bits >> 32;
#endif
            *p_dst++ = bits >> 24;
            *p_dst++ = bits >> 16;
            *p_dst++ = bits >> 8;
            *p_dst++ = bits;
            bits = henc->code;   /* OK not to clear high bits */
        }
        else
        {
            src -= 2;
            break;
        }
    }

    while (src != src_end)
    {
        cur_enc_code = encode_table[*src++];
        if (bits_used + cur_enc_code.bits < sizeof(bits) * 8)
        {
            bits <<= cur_enc_code.bits;
            bits |= cur_enc_code.code;
            bits_used += cur_enc_code.bits;
            continue;
        }
        else if (p_dst + sizeof(bits) <= dst_end)
        {
            bits <<= sizeof(bits) * 8 - bits_used;
            bits_used = cur_enc_code.bits - (sizeof(bits) * 8 - bits_used);
            bits |= cur_enc_code.code >> bits_used;
#if UINTPTR_MAX == 18446744073709551615ull
            *p_dst++ = bits >> 56;
            *p_dst++ = bits >> 48;
            *p_dst++ = bits >> 40;
            *p_dst++ = bits >> 32;
#endif
            *p_dst++ = bits >> 24;
            *p_dst++ = bits >> 16;
            *p_dst++ = bits >> 8;
            *p_dst++ = bits;
            bits = cur_enc_code.code;   /* OK not to clear high bits */
        }
        else
            return -1;
    }

    adj = bits_used + (-bits_used & 7);     /* Round up to 8 */
    if (bits_used && p_dst + (adj >> 3) <= dst_end)
    {
        bits <<= -bits_used & 7;            /* Align to byte boundary */
        bits |= ((1 << (-bits_used & 7)) - 1);  /* EOF */
        switch (adj >> 3)
        {                               /* Write out */
#if UINTPTR_MAX == 18446744073709551615ull
        case 8: *p_dst++ = bits >> 56;
        case 7: *p_dst++ = bits >> 48;
        case 6: *p_dst++ = bits >> 40;
        case 5: *p_dst++ = bits >> 32;
#endif
        case 4: *p_dst++ = bits >> 24;
        case 3: *p_dst++ = bits >> 16;
        case 2: *p_dst++ = bits >> 8;
        default: *p_dst++ = bits;
        }
        return p_dst - dst;
    }
    else if (p_dst + (adj >> 3) <= dst_end)
        return p_dst - dst;
    else
        return -1;
}
