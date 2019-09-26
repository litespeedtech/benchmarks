#include <assert.h>
#include <arpa/inet.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h> /* For ssize_t */

typedef struct {
  /* This points to the beginning of the buffer. The effective range
     of buffer is [begin, end). */
  uint8_t *begin;
  /* This points to the memory one byte beyond the end of the
     buffer. */
  uint8_t *end;
  /* The position indicator for effective start of the buffer. pos <=
     last must be hold. */
  uint8_t *pos;
  /* The position indicator for effective one beyond of the end of the
     buffer. last <= end must be hold. */
  uint8_t *last;
  /* Mark arbitrary position in buffer [begin, end) */
  uint8_t *mark;
} nghttp2_buf;

struct nghttp2_buf_chain;

typedef struct nghttp2_buf_chain nghttp2_buf_chain;

/* Chains 2 buffers */
struct nghttp2_buf_chain {
  /* Points to the subsequent buffer. NULL if there is no such
     buffer. */
  nghttp2_buf_chain *next;
  nghttp2_buf buf;
};

typedef struct {
  /* Points to the first buffer */
  nghttp2_buf_chain *head;
  /* Buffer pointer where write occurs. */
  nghttp2_buf_chain *cur;
  /* The buffer capacity of each buf */
  size_t chunk_length;
  /* The maximum number of nghttp2_buf_chain */
  size_t max_chunk;
  /* The number of nghttp2_buf_chain allocated */
  size_t chunk_used;
  /* The number of nghttp2_buf_chain to keep on reset */
  size_t chunk_keep;
  /* pos offset from begin in each buffers. On initialization and
     reset, buf->pos and buf->last are positioned at buf->begin +
     offset. */
  size_t offset;
} nghttp2_bufs;


#define nghttp2_buf_len(BUF) ((ssize_t)((BUF)->last - (BUF)->pos))
#define nghttp2_buf_avail(BUF) ((ssize_t)((BUF)->end - (BUF)->last))
#define nghttp2_buf_mark_avail(BUF) ((ssize_t)((BUF)->mark - (BUF)->last))
#define nghttp2_buf_cap(BUF) ((ssize_t)((BUF)->end - (BUF)->begin))
#define nghttp2_bufs_cur_avail(BUFS) nghttp2_buf_avail(&(BUFS)->cur->buf)

typedef struct {
  /* The number of bits in this code */
  uint32_t nbits;
  /* Huffman code aligned to LSB */
  uint32_t code;
} nghttp2_huff_sym;


const nghttp2_huff_sym huff_sym_table[] = {
  { 26, 0x3ffffbau },
  { 26, 0x3ffffbbu },
  { 26, 0x3ffffbcu },
  { 26, 0x3ffffbdu },
  { 26, 0x3ffffbeu },
  { 26, 0x3ffffbfu },
  { 26, 0x3ffffc0u },
  { 26, 0x3ffffc1u },
  { 26, 0x3ffffc2u },
  { 26, 0x3ffffc3u },
  { 26, 0x3ffffc4u },
  { 26, 0x3ffffc5u },
  { 26, 0x3ffffc6u },
  { 26, 0x3ffffc7u },
  { 26, 0x3ffffc8u },
  { 26, 0x3ffffc9u },
  { 26, 0x3ffffcau },
  { 26, 0x3ffffcbu },
  { 26, 0x3ffffccu },
  { 26, 0x3ffffcdu },
  { 26, 0x3ffffceu },
  { 26, 0x3ffffcfu },
  { 26, 0x3ffffd0u },
  { 26, 0x3ffffd1u },
  { 26, 0x3ffffd2u },
  { 26, 0x3ffffd3u },
  { 26, 0x3ffffd4u },
  { 26, 0x3ffffd5u },
  { 26, 0x3ffffd6u },
  { 26, 0x3ffffd7u },
  { 26, 0x3ffffd8u },
  { 26, 0x3ffffd9u },
  { 5, 0x6u },
  { 13, 0x1ffcu },
  { 9, 0x1f0u },
  { 14, 0x3ffcu },
  { 15, 0x7ffcu },
  { 6, 0x1eu },
  { 7, 0x64u },
  { 13, 0x1ffdu },
  { 10, 0x3fau },
  { 9, 0x1f1u },
  { 10, 0x3fbu },
  { 10, 0x3fcu },
  { 7, 0x65u },
  { 7, 0x66u },
  { 6, 0x1fu },
  { 5, 0x7u },
  { 4, 0x0u },
  { 4, 0x1u },
  { 4, 0x2u },
  { 5, 0x8u },
  { 6, 0x20u },
  { 6, 0x21u },
  { 6, 0x22u },
  { 6, 0x23u },
  { 6, 0x24u },
  { 6, 0x25u },
  { 6, 0x26u },
  { 8, 0xecu },
  { 17, 0x1fffcu },
  { 6, 0x27u },
  { 15, 0x7ffdu },
  { 10, 0x3fdu },
  { 15, 0x7ffeu },
  { 7, 0x67u },
  { 8, 0xedu },
  { 8, 0xeeu },
  { 7, 0x68u },
  { 8, 0xefu },
  { 7, 0x69u },
  { 7, 0x6au },
  { 9, 0x1f2u },
  { 8, 0xf0u },
  { 9, 0x1f3u },
  { 9, 0x1f4u },
  { 9, 0x1f5u },
  { 7, 0x6bu },
  { 7, 0x6cu },
  { 8, 0xf1u },
  { 8, 0xf2u },
  { 9, 0x1f6u },
  { 9, 0x1f7u },
  { 7, 0x6du },
  { 6, 0x28u },
  { 8, 0xf3u },
  { 9, 0x1f8u },
  { 9, 0x1f9u },
  { 8, 0xf4u },
  { 9, 0x1fau },
  { 9, 0x1fbu },
  { 11, 0x7fcu },
  { 26, 0x3ffffdau },
  { 11, 0x7fdu },
  { 14, 0x3ffdu },
  { 7, 0x6eu },
  { 18, 0x3fffeu },
  { 5, 0x9u },
  { 7, 0x6fu },
  { 5, 0xau },
  { 6, 0x29u },
  { 5, 0xbu },
  { 7, 0x70u },
  { 6, 0x2au },
  { 6, 0x2bu },
  { 5, 0xcu },
  { 8, 0xf5u },
  { 8, 0xf6u },
  { 6, 0x2cu },
  { 6, 0x2du },
  { 6, 0x2eu },
  { 5, 0xdu },
  { 6, 0x2fu },
  { 9, 0x1fcu },
  { 6, 0x30u },
  { 6, 0x31u },
  { 5, 0xeu },
  { 7, 0x71u },
  { 7, 0x72u },
  { 7, 0x73u },
  { 7, 0x74u },
  { 7, 0x75u },
  { 8, 0xf7u },
  { 17, 0x1fffdu },
  { 12, 0xffcu },
  { 17, 0x1fffeu },
  { 12, 0xffdu },
  { 26, 0x3ffffdbu },
  { 26, 0x3ffffdcu },
  { 26, 0x3ffffddu },
  { 26, 0x3ffffdeu },
  { 26, 0x3ffffdfu },
  { 26, 0x3ffffe0u },
  { 26, 0x3ffffe1u },
  { 26, 0x3ffffe2u },
  { 26, 0x3ffffe3u },
  { 26, 0x3ffffe4u },
  { 26, 0x3ffffe5u },
  { 26, 0x3ffffe6u },
  { 26, 0x3ffffe7u },
  { 26, 0x3ffffe8u },
  { 26, 0x3ffffe9u },
  { 26, 0x3ffffeau },
  { 26, 0x3ffffebu },
  { 26, 0x3ffffecu },
  { 26, 0x3ffffedu },
  { 26, 0x3ffffeeu },
  { 26, 0x3ffffefu },
  { 26, 0x3fffff0u },
  { 26, 0x3fffff1u },
  { 26, 0x3fffff2u },
  { 26, 0x3fffff3u },
  { 26, 0x3fffff4u },
  { 26, 0x3fffff5u },
  { 26, 0x3fffff6u },
  { 26, 0x3fffff7u },
  { 26, 0x3fffff8u },
  { 26, 0x3fffff9u },
  { 26, 0x3fffffau },
  { 26, 0x3fffffbu },
  { 26, 0x3fffffcu },
  { 26, 0x3fffffdu },
  { 26, 0x3fffffeu },
  { 26, 0x3ffffffu },
  { 25, 0x1ffff80u },
  { 25, 0x1ffff81u },
  { 25, 0x1ffff82u },
  { 25, 0x1ffff83u },
  { 25, 0x1ffff84u },
  { 25, 0x1ffff85u },
  { 25, 0x1ffff86u },
  { 25, 0x1ffff87u },
  { 25, 0x1ffff88u },
  { 25, 0x1ffff89u },
  { 25, 0x1ffff8au },
  { 25, 0x1ffff8bu },
  { 25, 0x1ffff8cu },
  { 25, 0x1ffff8du },
  { 25, 0x1ffff8eu },
  { 25, 0x1ffff8fu },
  { 25, 0x1ffff90u },
  { 25, 0x1ffff91u },
  { 25, 0x1ffff92u },
  { 25, 0x1ffff93u },
  { 25, 0x1ffff94u },
  { 25, 0x1ffff95u },
  { 25, 0x1ffff96u },
  { 25, 0x1ffff97u },
  { 25, 0x1ffff98u },
  { 25, 0x1ffff99u },
  { 25, 0x1ffff9au },
  { 25, 0x1ffff9bu },
  { 25, 0x1ffff9cu },
  { 25, 0x1ffff9du },
  { 25, 0x1ffff9eu },
  { 25, 0x1ffff9fu },
  { 25, 0x1ffffa0u },
  { 25, 0x1ffffa1u },
  { 25, 0x1ffffa2u },
  { 25, 0x1ffffa3u },
  { 25, 0x1ffffa4u },
  { 25, 0x1ffffa5u },
  { 25, 0x1ffffa6u },
  { 25, 0x1ffffa7u },
  { 25, 0x1ffffa8u },
  { 25, 0x1ffffa9u },
  { 25, 0x1ffffaau },
  { 25, 0x1ffffabu },
  { 25, 0x1ffffacu },
  { 25, 0x1ffffadu },
  { 25, 0x1ffffaeu },
  { 25, 0x1ffffafu },
  { 25, 0x1ffffb0u },
  { 25, 0x1ffffb1u },
  { 25, 0x1ffffb2u },
  { 25, 0x1ffffb3u },
  { 25, 0x1ffffb4u },
  { 25, 0x1ffffb5u },
  { 25, 0x1ffffb6u },
  { 25, 0x1ffffb7u },
  { 25, 0x1ffffb8u },
  { 25, 0x1ffffb9u },
  { 25, 0x1ffffbau },
  { 25, 0x1ffffbbu },
  { 25, 0x1ffffbcu },
  { 25, 0x1ffffbdu },
  { 25, 0x1ffffbeu },
  { 25, 0x1ffffbfu },
  { 25, 0x1ffffc0u },
  { 25, 0x1ffffc1u },
  { 25, 0x1ffffc2u },
  { 25, 0x1ffffc3u },
  { 25, 0x1ffffc4u },
  { 25, 0x1ffffc5u },
  { 25, 0x1ffffc6u },
  { 25, 0x1ffffc7u },
  { 25, 0x1ffffc8u },
  { 25, 0x1ffffc9u },
  { 25, 0x1ffffcau },
  { 25, 0x1ffffcbu },
  { 25, 0x1ffffccu },
  { 25, 0x1ffffcdu },
  { 25, 0x1ffffceu },
  { 25, 0x1ffffcfu },
  { 25, 0x1ffffd0u },
  { 25, 0x1ffffd1u },
  { 25, 0x1ffffd2u },
  { 25, 0x1ffffd3u },
  { 25, 0x1ffffd4u },
  { 25, 0x1ffffd5u },
  { 25, 0x1ffffd6u },
  { 25, 0x1ffffd7u },
  { 25, 0x1ffffd8u },
  { 25, 0x1ffffd9u },
  { 25, 0x1ffffdau },
  { 25, 0x1ffffdbu },
  { 25, 0x1ffffdcu }
};

#define nghttp2_bufs_fast_orb_hold(BUFS, B)     \
  do {                                          \
    *(BUFS)->cur->buf.last |= B;                \
  } while(0)

#define nghttp2_bufs_fast_addb_hold(BUFS, B)     \
  do {                                           \
    *(BUFS)->cur->buf.last = B;                  \
  } while(0)

#define nghttp2_bufs_fast_orb(BUFS, B)          \
  do {                                          \
    *(BUFS)->cur->buf.last++ |= B;              \
  } while(0)

enum {
  NGHTTP2_ERR_INVALID_ARGUMENT = -501,
  NGHTTP2_ERR_BUFFER_ERROR = -502,
  NGHTTP2_ERR_NOMEM = -901,
};

void nghttp2_buf_init(nghttp2_buf *buf)
{
  buf->begin = NULL;
  buf->end = NULL;
  buf->pos = NULL;
  buf->last = NULL;
  buf->mark = NULL;
}

#define nghttp2_max(A, B) ((A) > (B) ? (A) : (B))

int nghttp2_buf_reserve(nghttp2_buf *buf, size_t new_cap)
{
  uint8_t *ptr;
  size_t cap;
  cap = nghttp2_buf_cap(buf);
  if(cap >= new_cap) {
    return 0;
  }
  new_cap = nghttp2_max(new_cap, cap * 2);
  ptr = realloc(buf->begin, new_cap);
  if(ptr == NULL) {
    return NGHTTP2_ERR_NOMEM;
  }
  buf->pos = ptr + (buf->pos - buf->begin);
  buf->last = ptr + (buf->last - buf->begin);
  buf->mark = ptr + (buf->mark - buf->begin);
  buf->begin = ptr;
  buf->end = ptr + new_cap;
  return 0;
}

int nghttp2_buf_init2(nghttp2_buf *buf, size_t initial)
{
  nghttp2_buf_init(buf);
  return nghttp2_buf_reserve(buf, initial);
}

static int buf_chain_new(nghttp2_buf_chain **chain, size_t chunk_length)
{
  int rv;
  *chain = malloc(sizeof(nghttp2_buf_chain));
  if(*chain == NULL) {
    return NGHTTP2_ERR_NOMEM;
  }
  (*chain)->next = NULL;
  rv = nghttp2_buf_init2(&(*chain)->buf, chunk_length);
  if(rv != 0) {
    free(*chain);
    return NGHTTP2_ERR_NOMEM;
  }
  return 0;
}

#define nghttp2_buf_shift_right(BUF, AMT) \
  do {                                    \
    (BUF)->pos += AMT;                    \
    (BUF)->last += AMT;                   \
  } while(0)

static int bufs_alloc_chain(nghttp2_bufs *bufs)
{
  int rv;
  nghttp2_buf_chain *chain;
  if(bufs->cur->next) {
    bufs->cur = bufs->cur->next;
    return 0;
  }
  if(bufs->max_chunk == bufs->chunk_used) {
    return NGHTTP2_ERR_BUFFER_ERROR;
  }
  rv = buf_chain_new(&chain, bufs->chunk_length);
  if(rv != 0) {
    return rv;
  }
  ++bufs->chunk_used;
  bufs->cur->next = chain;
  bufs->cur = chain;
  nghttp2_buf_shift_right(&bufs->cur->buf, bufs->offset);
  return 0;
}

static int bufs_ensure_addb(nghttp2_bufs *bufs)
{
  int rv;
  nghttp2_buf *buf;
  buf = &bufs->cur->buf;
  if(nghttp2_buf_avail(buf) > 0) {
    return 0;
  }
  rv = bufs_alloc_chain(bufs);
  if(rv != 0) {
    return rv;
  }
  return 0;
}

int nghttp2_bufs_addb_hold(nghttp2_bufs *bufs, uint8_t b)
{
  int rv;
  rv = bufs_ensure_addb(bufs);
  if(rv != 0) {
    return rv;
  }
  *bufs->cur->buf.last = b;
  return 0;
}

int nghttp2_bufs_orb_hold(nghttp2_bufs *bufs, uint8_t b)
{
  int rv;
  rv = bufs_ensure_addb(bufs);
  if(rv != 0) {
    return rv;
  }
  *bufs->cur->buf.last |= b;
  return 0;
}

int nghttp2_bufs_orb(nghttp2_bufs *bufs, uint8_t b)
{
  int rv;
  rv = bufs_ensure_addb(bufs);
  if(rv != 0) {
    return rv;
  }
  *bufs->cur->buf.last++ |= b;
  return 0;
}

/*
 * Encodes huffman code |sym| into |*dest_ptr|, whose least |rembits|
 * bits are not filled yet.  The |rembits| must be in range [1, 8],
 * inclusive.  At the end of the process, the |*dest_ptr| is updated
 * and points where next output should be placed. The number of
 * unfilled bits in the pointed location is returned.
 */
static ssize_t huff_encode_sym(nghttp2_bufs *bufs, size_t *avail_ptr,
                               size_t rembits,
                               const nghttp2_huff_sym *sym)
{
  int rv;
  size_t nbits = sym->nbits;

  for(;;) {
    if(rembits > nbits) {
      if(*avail_ptr) {
        nghttp2_bufs_fast_orb_hold(bufs, sym->code << (rembits - nbits));
      } else {
        rv = nghttp2_bufs_orb_hold(bufs, sym->code << (rembits - nbits));
        if(rv != 0) {
          return rv;
        }

        *avail_ptr = nghttp2_bufs_cur_avail(bufs);
      }

      rembits -= nbits;

      break;
    }

    if(*avail_ptr) {
      nghttp2_bufs_fast_orb(bufs, sym->code >> (nbits - rembits));
      --*avail_ptr;
    } else {
      rv = nghttp2_bufs_orb(bufs, sym->code >> (nbits - rembits));
      if(rv != 0) {
        return rv;
      }

      *avail_ptr = nghttp2_bufs_cur_avail(bufs);
    }

    nbits -= rembits;
    rembits = 8;

    if(nbits == 0) {
      break;
    }

    if(*avail_ptr) {
      nghttp2_bufs_fast_addb_hold(bufs, 0);
    } else {
      rv = nghttp2_bufs_addb_hold(bufs, 0);
      if(rv != 0) {
        return rv;
      }

      *avail_ptr = nghttp2_bufs_cur_avail(bufs);
    }
  }
  return rembits;
}

int nghttp2_hd_huff_encode(nghttp2_bufs *bufs, const uint8_t *src,
                           size_t srclen) {
  int rv;
  ssize_t rembits = 8;
  size_t i;
  size_t avail;

  avail = nghttp2_bufs_cur_avail(bufs);

  for (i = 0; i < srclen; ++i) {
    const nghttp2_huff_sym *sym = &huff_sym_table[src[i]];
    if (rembits == 8) {
      if (avail) {
        nghttp2_bufs_fast_addb_hold(bufs, 0);
      } else {
        rv = nghttp2_bufs_addb_hold(bufs, 0);
        if (rv != 0) {
          return rv;
        }
        avail = nghttp2_bufs_cur_avail(bufs);
      }
    }
    rembits = huff_encode_sym(bufs, &avail, (size_t)rembits, sym);
    if (rembits < 0) {
      return (int)rembits;
    }
  }
  /* 256 is special terminal symbol, pad with its prefix */
  if (rembits < 8) {
    /* if rembits < 8, we should have at least 1 buffer space
       available */
    const nghttp2_huff_sym *sym = &huff_sym_table[256];
    assert(avail);
    /* Caution we no longer adjust avail here */
    nghttp2_bufs_fast_orb(
        bufs, (uint8_t)(sym->code >> (sym->nbits - (size_t)rembits)));
  }

  return 0;
}

void nghttp2_buf_reset(nghttp2_buf *buf)
{
  buf->pos = buf->last = buf->mark = buf->begin;
}

int nghttp2_bufs_init3(nghttp2_bufs *bufs, size_t chunk_length,
                       size_t max_chunk, size_t chunk_keep, size_t offset)
{
  int rv;
  nghttp2_buf_chain *chain;

  if(chunk_keep == 0 || max_chunk < chunk_keep || chunk_length < offset) {
    return NGHTTP2_ERR_INVALID_ARGUMENT;
  }

  rv = buf_chain_new(&chain, chunk_length);
  if(rv != 0) {
    return rv;
  }

  bufs->offset = offset;

  bufs->head = chain;
  bufs->cur = bufs->head;

  nghttp2_buf_shift_right(&bufs->cur->buf, offset);

  bufs->chunk_length = chunk_length;
  bufs->chunk_used = 1;
  bufs->max_chunk = max_chunk;
  bufs->chunk_keep = chunk_keep;

  return 0;
}

void nghttp2_buf_free(nghttp2_buf *buf)
{
  free(buf->begin);
}

static void buf_chain_del(nghttp2_buf_chain *chain)
{
  nghttp2_buf_free(&chain->buf);
  free(chain);
}

void nghttp2_bufs_reset(nghttp2_bufs *bufs)
{
  nghttp2_buf_chain *chain, *ci;
  size_t k;
  k = bufs->chunk_keep;
  for(ci = bufs->head; ci; ci = ci->next) {
    nghttp2_buf_reset(&ci->buf);
    nghttp2_buf_shift_right(&ci->buf, bufs->offset);
    if(--k == 0) {
      break;
    }
  }
  if(ci) {
    chain = ci->next;
    ci->next = NULL;
    for(ci = chain; ci;) {
      chain = ci->next;
      buf_chain_del(ci);
      ci = chain;
    }
    bufs->chunk_used = bufs->chunk_keep;
  }
  bufs->cur = bufs->head;
}
