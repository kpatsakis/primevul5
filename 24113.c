static int chacha20_poly1305_tls_cipher(EVP_CIPHER_CTX *ctx, unsigned char *out,
                                        const unsigned char *in, size_t len)
{
    EVP_CHACHA_AEAD_CTX *actx = aead_data(ctx);
    size_t tail, tohash_len, buf_len, plen = actx->tls_payload_length;
    unsigned char *buf, *tohash, *ctr, storage[sizeof(zero) + 32];

    if (len != plen + POLY1305_BLOCK_SIZE)
        return -1;

    buf = storage + ((0 - (size_t)storage) & 15);   /* align */
    ctr = buf + CHACHA_BLK_SIZE;
    tohash = buf + CHACHA_BLK_SIZE - POLY1305_BLOCK_SIZE;

#   ifdef XOR128_HELPERS
    if (plen <= 3 * CHACHA_BLK_SIZE) {
        actx->key.counter[0] = 0;
        buf_len = (plen + 2 * CHACHA_BLK_SIZE - 1) & (0 - CHACHA_BLK_SIZE);
        ChaCha20_ctr32(buf, zero, buf_len, actx->key.key.d,
                       actx->key.counter);
        Poly1305_Init(POLY1305_ctx(actx), buf);
        actx->key.partial_len = 0;
        memcpy(tohash, actx->tls_aad, POLY1305_BLOCK_SIZE);
        tohash_len = POLY1305_BLOCK_SIZE;
        actx->len.aad = EVP_AEAD_TLS1_AAD_LEN;
        actx->len.text = plen;

        if (plen) {
            if (ctx->encrypt)
                ctr = xor128_encrypt_n_pad(out, in, ctr, plen);
            else
                ctr = xor128_decrypt_n_pad(out, in, ctr, plen);

            in += plen;
            out += plen;
            tohash_len = (size_t)(ctr - tohash);
        }
    }
#   else
    if (plen <= CHACHA_BLK_SIZE) {
        size_t i;

        actx->key.counter[0] = 0;
        ChaCha20_ctr32(buf, zero, (buf_len = 2 * CHACHA_BLK_SIZE),
                       actx->key.key.d, actx->key.counter);
        Poly1305_Init(POLY1305_ctx(actx), buf);
        actx->key.partial_len = 0;
        memcpy(tohash, actx->tls_aad, POLY1305_BLOCK_SIZE);
        tohash_len = POLY1305_BLOCK_SIZE;
        actx->len.aad = EVP_AEAD_TLS1_AAD_LEN;
        actx->len.text = plen;

        if (ctx->encrypt) {
            for (i = 0; i < plen; i++) {
                out[i] = ctr[i] ^= in[i];
            }
        } else {
            for (i = 0; i < plen; i++) {
                unsigned char c = in[i];
                out[i] = ctr[i] ^ c;
                ctr[i] = c;
            }
        }

        in += i;
        out += i;

        tail = (0 - i) & (POLY1305_BLOCK_SIZE - 1);
        memset(ctr + i, 0, tail);
        ctr += i + tail;
        tohash_len += i + tail;
    }
#   endif
    else {
        actx->key.counter[0] = 0;
        ChaCha20_ctr32(buf, zero, (buf_len = CHACHA_BLK_SIZE),
                       actx->key.key.d, actx->key.counter);
        Poly1305_Init(POLY1305_ctx(actx), buf);
        actx->key.counter[0] = 1;
        actx->key.partial_len = 0;
        Poly1305_Update(POLY1305_ctx(actx), actx->tls_aad, POLY1305_BLOCK_SIZE);
        tohash = ctr;
        tohash_len = 0;
        actx->len.aad = EVP_AEAD_TLS1_AAD_LEN;
        actx->len.text = plen;

        if (ctx->encrypt) {
            ChaCha20_ctr32(out, in, plen, actx->key.key.d, actx->key.counter);
            Poly1305_Update(POLY1305_ctx(actx), out, plen);
        } else {
            Poly1305_Update(POLY1305_ctx(actx), in, plen);
            ChaCha20_ctr32(out, in, plen, actx->key.key.d, actx->key.counter);
        }

        in += plen;
        out += plen;
        tail = (0 - plen) & (POLY1305_BLOCK_SIZE - 1);
        Poly1305_Update(POLY1305_ctx(actx), zero, tail);
    }

    {
        const union {
            long one;
            char little;
        } is_endian = { 1 };

        if (is_endian.little) {
            memcpy(ctr, (unsigned char *)&actx->len, POLY1305_BLOCK_SIZE);
        } else {
            ctr[0]  = (unsigned char)(actx->len.aad);
            ctr[1]  = (unsigned char)(actx->len.aad>>8);
            ctr[2]  = (unsigned char)(actx->len.aad>>16);
            ctr[3]  = (unsigned char)(actx->len.aad>>24);
            ctr[4]  = (unsigned char)(actx->len.aad>>32);
            ctr[5]  = (unsigned char)(actx->len.aad>>40);
            ctr[6]  = (unsigned char)(actx->len.aad>>48);
            ctr[7]  = (unsigned char)(actx->len.aad>>56);

            ctr[8]  = (unsigned char)(actx->len.text);
            ctr[9]  = (unsigned char)(actx->len.text>>8);
            ctr[10] = (unsigned char)(actx->len.text>>16);
            ctr[11] = (unsigned char)(actx->len.text>>24);
            ctr[12] = (unsigned char)(actx->len.text>>32);
            ctr[13] = (unsigned char)(actx->len.text>>40);
            ctr[14] = (unsigned char)(actx->len.text>>48);
            ctr[15] = (unsigned char)(actx->len.text>>56);
        }
        tohash_len += POLY1305_BLOCK_SIZE;
    }

    Poly1305_Update(POLY1305_ctx(actx), tohash, tohash_len);
    OPENSSL_cleanse(buf, buf_len);
    Poly1305_Final(POLY1305_ctx(actx), ctx->encrypt ? actx->tag
                                                    : tohash);

    actx->tls_payload_length = NO_TLS_PAYLOAD_LENGTH;

    if (ctx->encrypt) {
        memcpy(out, actx->tag, POLY1305_BLOCK_SIZE);
    } else {
        if (CRYPTO_memcmp(tohash, in, POLY1305_BLOCK_SIZE)) {
            memset(out - (len - POLY1305_BLOCK_SIZE), 0,
                   len - POLY1305_BLOCK_SIZE);
            return -1;
        }
    }

    return len;
}