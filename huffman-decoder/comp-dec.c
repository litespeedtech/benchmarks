#include <assert.h>
#include <byteswap.h>
#include <arpa/inet.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> /* For ssize_t */


int
lshpack_dec_huff_decode (const unsigned char *src, int src_len,
                                            unsigned char *dst, int dst_len);

int
lshpack_dec_huff_decode_full (const unsigned char *src, int src_len,
                                            unsigned char *dst, int dst_len);

int
ngx_http_v2_huff_decode (const unsigned char *src, int len,
                                            unsigned char *dst, int dst_len);

int
main (int argc, char **argv)
{
    size_t in_sz;
    int count, i, rv;
    FILE *in;
    int (*decode)(const unsigned char *, int, unsigned char *, int);
    unsigned char in_buf[0x1000];
    unsigned char out_buf[0x4000];

    if (argc != 4)
    {
        fprintf(stderr,
                "Usage: %s $file $count $mode\n"
                "\n"
                "  $mode is either litespeed, litespeed-full, or nginx\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    if (strcasecmp(argv[3], "litespeed") == 0)
        decode = lshpack_dec_huff_decode;
    else if (strcasecmp(argv[3], "litespeed-full") == 0)
        decode = lshpack_dec_huff_decode_full;
    else if (strcasecmp(argv[3], "nginx") == 0)
        decode = ngx_http_v2_huff_decode;
    else
    {
        fprintf(stderr, "Mode `%s' is invalid.  Specify either one of the following:\n"
            "  litespeed\n"
            "  litespeed-full\n"
            "  nginx\n"
            , argv[3]);
        exit(EXIT_FAILURE);
    }

    in = fopen(argv[1], "rb");
    if (!in)
    {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    in_sz = fread(in_buf, 1, sizeof(in_buf), in);
    if (in_sz == 0 || in_sz == sizeof(in_buf))
    {
        fprintf(stderr, "input file is either too short or too long\n");
        exit(EXIT_FAILURE);
    }
    (void) fclose(in);

    count = atoi(argv[2]);
    if (!count)
        count = 1;

    rv = decode(in_buf, in_sz, out_buf, sizeof(out_buf));
    if (rv < 0)
    {
        fprintf(stderr, "decode-%s returned %d\n", argv[3], rv);
        exit(EXIT_FAILURE);
    }

    for (i = 0; i < count; ++i)
    {
        rv = decode(in_buf, in_sz, out_buf, sizeof(out_buf));
        (void) rv;
    }

    exit(EXIT_SUCCESS);
}
