Code in litespeed.c is from ls-hpack [1] revision c7498f361ea

Code in nginx.c is adapted from nginx 1.16.1 [2]

Three input files of different sizes are provided:

    Name              Plaintext
    ---------------   -------------------------

    litespeed.huff    LiteSpeed

			Just a small string.

    x-fb-debug.huff   mEO7bfwFStBMwJWfW4pmg2XL25AswjrVlfcfYbxkcS2ssduZmiKoipMH9XwoTGkb+Qnq9bcjwWbwDQzsea/vMQ==

			This string is from the Facebook response QIF [3].

    x-fb-backslash.huff
		      mEO7bfwFStBMwJWfW4pmg2XL25AswjrVlfcfYbxkcS2ssduZ\\iKoipMH9XwoTGkb+Qnq9bcjwWbwDQzsea/vMQ==

			Same as above, but with a backslash in the middle.
			This is so that fallback code is exercised.

    idle.huff         A few beginning paragraphs of the Idle Thoughts of an
                        Idle Fellow by Jerome K. Jerome [4].

			Newlines have been removed, as they are not likely to
			be found in the normal input.


1. https://github.com/litespeedtech/ls-hpack
2. https://hg.nginx.org/nginx/file/stable-1.16/src/http/v2/ngx_http_v2_huff_decode.c
3. https://github.com/qpackers/qifs/blob/master/qifs/fb-resp.qif
4. https://www.gutenberg.org/ebooks/849
