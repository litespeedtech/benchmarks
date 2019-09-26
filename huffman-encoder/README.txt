This is the HPACK Huffman encoder benchmark.

Type `make' to compile the driver, comp-enc.  bench.pl is a useful script
to run a combination of input and implementations.

Code in litespeed.c is from ls-hpack [1] revision a1f1ea8ce298f
Code in nginx.c is adapted from nginx 1.16.1 [2]
Code in h2o.c is adapted from h2o [3] revision d5d979cc5ef92
Code in nghttp2.c is adapted from nghttp2 [4] revision 0e1d0400d86a

Four input files of different sizes are provided:

    Name              Plaintext
    ---------------   -------------------------

    litespeed.txt     LiteSpeed

			Just a small string.

    x-fb-debug.txt    mEO7bfwFStBMwJWfW4pmg2XL25AswjrVlfcfYbxkcS2ssduZmiKoipMH9XwoTGkb+Qnq9bcjwWbwDQzsea/vMQ==

			This string is from the Facebook response QIF [5].

    x-fb-backslash.txt
		      mEO7bfwFStBMwJWfW4pmg2XL25AswjrVlfcfYbxkcS2ssduZ\\iKoipMH9XwoTGkb+Qnq9bcjwWbwDQzsea/vMQ==

			Same as above, but with a backslash in the middle.
			This is so that fallback code is exercised.

    idle.huff         A few beginning paragraphs of the Idle Thoughts of an
                        Idle Fellow by Jerome K. Jerome [6].

			Newlines have been removed, as they are not likely to
			be found in the normal input.


1. https://github.com/litespeedtech/ls-hpack
2. https://hg.nginx.org/nginx/file/stable-1.16/src/http/v2/ngx_http_v2_huff_encode.c
3. https://github.com/h2o/h2o/blob/master/lib/http2/hpack.c
4. https://github.com/nghttp2/nghttp2/blob/master/lib/nghttp2_hd_huffman.c
5. https://github.com/qpackers/qifs/blob/master/qifs/fb-resp.qif
6. https://www.gutenberg.org/ebooks/849
