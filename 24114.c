static int chacha20_poly1305_init_key(EVP_CIPHER_CTX *ctx,
                                      const unsigned char *inkey,
                                      const unsigned char *iv, int enc)
{
    EVP_CHACHA_AEAD_CTX *actx = aead_data(ctx);

    if (!inkey && !iv)
        return 1;

    actx->len.aad = 0;
    actx->len.text = 0;
    actx->aad = 0;
    actx->mac_inited = 0;
    actx->tls_payload_length = NO_TLS_PAYLOAD_LENGTH;

    if (iv != NULL) {
        unsigned char temp[CHACHA_CTR_SIZE] = { 0 };

        /* pad on the left */
        if (actx->nonce_len <= CHACHA_CTR_SIZE)
            memcpy(temp + CHACHA_CTR_SIZE - actx->nonce_len, iv,
                   actx->nonce_len);

        chacha_init_key(ctx, inkey, temp, enc);

        actx->nonce[0] = actx->key.counter[1];
        actx->nonce[1] = actx->key.counter[2];
        actx->nonce[2] = actx->key.counter[3];
    } else {
        chacha_init_key(ctx, inkey, NULL, enc);
    }

    return 1;
}