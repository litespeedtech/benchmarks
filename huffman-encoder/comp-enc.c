#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int
lshpack_enc_huff_encode_orig (const unsigned char *src, int src_len,
                                        unsigned char *dst, int dst_len);

int
lshpack_enc_huff_encode (const unsigned char *src,
    int len, unsigned char *dst, int dst_len);

int
ngx_http_v2_huff_encode(const unsigned char *src, int len, unsigned char *dst,
                                                        int dst_len_UNUSED);

int
h2o_hpack_encode_huffman(const unsigned char *src, int len, unsigned char *_dst,
								int UNUSED);

typedef struct {
  struct nghttp2_buf_chain *head;
  struct nghttp2_buf_chain *cur;
  size_t chunk_length;
  size_t max_chunk;
  size_t chunk_used;
  size_t chunk_keep;
  size_t offset;
} nghttp2_bufs;

int
nghttp2_hd_huff_encode(nghttp2_bufs *bufs, const unsigned char *src,
                           size_t srclen);
void nghttp2_bufs_reset(nghttp2_bufs *bufs);
int nghttp2_bufs_init3(nghttp2_bufs *bufs, size_t chunk_length,
                       size_t max_chunk, size_t chunk_keep, size_t offset);

static nghttp2_bufs s_bufs;

/* When the function is called, the buffer has been already initialized */
static int
nghttp2_encode_wrapper (const unsigned char *src, int src_len,
                                            unsigned char *dst, int dst_len)
{
    (void) nghttp2_hd_huff_encode(&s_bufs, src, (size_t) src_len);
    nghttp2_bufs_reset(&s_bufs);
    return 0;   /* XXX We ignore return value */
}


int
main (int argc, char **argv)
{
    size_t in_sz;
    int count, i, rv;
    FILE *in;
    int (*encode)(const unsigned char *, int, unsigned char *, int);
    unsigned char in_buf[0x1000];
    unsigned char out_buf[0x4000];

    if (argc != 4)
    {
        fprintf(stderr,
                "Usage: %s $file $count $mode\n"
                "\n"
                "  $mode is either litespeed, litespeed-orig, h2o, nghttp2, or nginx\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    if (strcasecmp(argv[3], "litespeed") == 0)
        encode = lshpack_enc_huff_encode;
    else if (strcasecmp(argv[3], "litespeed-orig") == 0)
        encode = lshpack_enc_huff_encode_orig;
    else if (strcasecmp(argv[3], "h2o") == 0)
        encode = h2o_hpack_encode_huffman;
    else if (strcasecmp(argv[3], "nginx") == 0)
        encode = ngx_http_v2_huff_encode;
    else if (strcasecmp(argv[3], "nghttp2") == 0)
    {
        nghttp2_bufs_init3(&s_bufs, 0x4000, 1, 1, 0);
        encode = nghttp2_encode_wrapper;
    }
    else
    {
        fprintf(stderr, "Mode `%s' is invalid.  Specify either one of the following:\n"
            "  h2o\n"
            "  litespeed\n"
            "  nghttp2\n"
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

    rv = encode(in_buf, in_sz, out_buf, sizeof(out_buf));
    if (rv < 0)
    {
        fprintf(stderr, "decode-%s returned %d\n", argv[3], rv);
        exit(EXIT_FAILURE);
    }

    for (i = 0; i < count; ++i)
    {
        rv = encode(in_buf, in_sz, out_buf, sizeof(out_buf));
        (void) rv;
    }

    exit(EXIT_SUCCESS);
}
