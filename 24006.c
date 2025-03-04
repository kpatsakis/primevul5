BIO *PKCS7_dataDecode(PKCS7 *p7, EVP_PKEY *pkey, BIO *in_bio, X509 *pcert)
{
    int i, j;
    BIO *out = NULL, *btmp = NULL, *etmp = NULL, *bio = NULL;
    unsigned char *tmp = NULL;
    X509_ALGOR *xa;
    ASN1_OCTET_STRING *data_body = NULL;
    const EVP_MD *evp_md;
    const EVP_CIPHER *evp_cipher = NULL;
    EVP_CIPHER_CTX *evp_ctx = NULL;
    X509_ALGOR *enc_alg = NULL;
    STACK_OF(X509_ALGOR) *md_sk = NULL;
    STACK_OF(PKCS7_RECIP_INFO) *rsk = NULL;
    PKCS7_RECIP_INFO *ri = NULL;

    if (p7 == NULL) {
        PKCS7err(PKCS7_F_PKCS7_DATADECODE, PKCS7_R_INVALID_NULL_POINTER);
        return NULL;
    }

    if (p7->d.ptr == NULL) {
        PKCS7err(PKCS7_F_PKCS7_DATADECODE, PKCS7_R_NO_CONTENT);
        return NULL;
    }

    i = OBJ_obj2nid(p7->type);
    p7->state = PKCS7_S_HEADER;

    switch (i) {
    case NID_pkcs7_signed:
        /*
         * p7->d.sign->contents is a PKCS7 structure consisting of a contentType
         * field and optional content.
         * data_body is NULL if that structure has no (=detached) content
         * or if the contentType is wrong (i.e., not "data").
         */
        data_body = PKCS7_get_octet_string(p7->d.sign->contents);
        md_sk = p7->d.sign->md_algs;
        break;
    case NID_pkcs7_signedAndEnveloped:
        rsk = p7->d.signed_and_enveloped->recipientinfo;
        md_sk = p7->d.signed_and_enveloped->md_algs;
        /* data_body is NULL if the optional EncryptedContent is missing. */
        data_body = p7->d.signed_and_enveloped->enc_data->enc_data;
        enc_alg = p7->d.signed_and_enveloped->enc_data->algorithm;
        evp_cipher = EVP_get_cipherbyobj(enc_alg->algorithm);
        if (evp_cipher == NULL) {
            PKCS7err(PKCS7_F_PKCS7_DATADECODE,
                     PKCS7_R_UNSUPPORTED_CIPHER_TYPE);
            goto err;
        }
        break;
    case NID_pkcs7_enveloped:
        rsk = p7->d.enveloped->recipientinfo;
        enc_alg = p7->d.enveloped->enc_data->algorithm;
        /* data_body is NULL if the optional EncryptedContent is missing. */
        data_body = p7->d.enveloped->enc_data->enc_data;
        evp_cipher = EVP_get_cipherbyobj(enc_alg->algorithm);
        if (evp_cipher == NULL) {
            PKCS7err(PKCS7_F_PKCS7_DATADECODE,
                     PKCS7_R_UNSUPPORTED_CIPHER_TYPE);
            goto err;
        }
        break;
    default:
        PKCS7err(PKCS7_F_PKCS7_DATADECODE, PKCS7_R_UNSUPPORTED_CONTENT_TYPE);
        goto err;
    }

    /* Detached content must be supplied via in_bio instead. */
    if (data_body == NULL && in_bio == NULL) {
        PKCS7err(PKCS7_F_PKCS7_DATADECODE, PKCS7_R_NO_CONTENT);
        goto err;
    }

    /* We will be checking the signature */
    if (md_sk != NULL) {
        for (i = 0; i < sk_X509_ALGOR_num(md_sk); i++) {
            xa = sk_X509_ALGOR_value(md_sk, i);
            if ((btmp = BIO_new(BIO_f_md())) == NULL) {
                PKCS7err(PKCS7_F_PKCS7_DATADECODE, ERR_R_BIO_LIB);
                goto err;
            }

            j = OBJ_obj2nid(xa->algorithm);
            evp_md = EVP_get_digestbynid(j);
            if (evp_md == NULL) {
                PKCS7err(PKCS7_F_PKCS7_DATADECODE,
                         PKCS7_R_UNKNOWN_DIGEST_TYPE);
                goto err;
            }

            BIO_set_md(btmp, evp_md);
            if (out == NULL)
                out = btmp;
            else
                BIO_push(out, btmp);
            btmp = NULL;
        }
    }

    if (evp_cipher != NULL) {
#if 0
        unsigned char key[EVP_MAX_KEY_LENGTH];
        unsigned char iv[EVP_MAX_IV_LENGTH];
        unsigned char *p;
        int keylen, ivlen;
        int max;
        X509_OBJECT ret;
#endif
        unsigned char *tkey = NULL;
        int tkeylen;
        int jj;

        if ((etmp = BIO_new(BIO_f_cipher())) == NULL) {
            PKCS7err(PKCS7_F_PKCS7_DATADECODE, ERR_R_BIO_LIB);
            goto err;
        }

        /*
         * It was encrypted, we need to decrypt the secret key with the
         * private key
         */

        /*
         * Find the recipientInfo which matches the passed certificate (if
         * any)
         */

        if (pcert) {
            for (i = 0; i < sk_PKCS7_RECIP_INFO_num(rsk); i++) {
                ri = sk_PKCS7_RECIP_INFO_value(rsk, i);
                if (!pkcs7_cmp_ri(ri, pcert))
                    break;
                ri = NULL;
            }
            if (ri == NULL) {
                PKCS7err(PKCS7_F_PKCS7_DATADECODE,
                         PKCS7_R_NO_RECIPIENT_MATCHES_CERTIFICATE);
                goto err;
            }
        }

        jj = EVP_PKEY_size(pkey);
        tmp = (unsigned char *)OPENSSL_malloc(jj + 10);
        if (tmp == NULL) {
            PKCS7err(PKCS7_F_PKCS7_DATADECODE, ERR_R_MALLOC_FAILURE);
            goto err;
        }

        /* If we haven't got a certificate try each ri in turn */

        if (pcert == NULL) {
            /*
             * Temporary storage in case EVP_PKEY_decrypt overwrites output
             * buffer on error.
             */
            unsigned char *tmp2;
            tmp2 = OPENSSL_malloc(jj);
            if (!tmp2)
                goto err;
            jj = -1;
            /*
             * Always attempt to decrypt all cases to avoid leaking timing
             * information about a successful decrypt.
             */
            for (i = 0; i < sk_PKCS7_RECIP_INFO_num(rsk); i++) {
                int tret;
                ri = sk_PKCS7_RECIP_INFO_value(rsk, i);
                tret = EVP_PKEY_decrypt(tmp2,
                                        M_ASN1_STRING_data(ri->enc_key),
                                        M_ASN1_STRING_length(ri->enc_key),
                                        pkey);
                if (tret > 0) {
                    memcpy(tmp, tmp2, tret);
                    OPENSSL_cleanse(tmp2, tret);
                    jj = tret;
                }
                ERR_clear_error();
            }
            OPENSSL_free(tmp2);
        } else {
            jj = EVP_PKEY_decrypt(tmp,
                                  M_ASN1_STRING_data(ri->enc_key),
                                  M_ASN1_STRING_length(ri->enc_key), pkey);
            ERR_clear_error();
        }

        evp_ctx = NULL;
        BIO_get_cipher_ctx(etmp, &evp_ctx);
        if (EVP_CipherInit_ex(evp_ctx, evp_cipher, NULL, NULL, NULL, 0) <= 0)
            goto err;
        if (EVP_CIPHER_asn1_to_param(evp_ctx, enc_alg->parameter) < 0)
            goto err;
        /* Generate random key to counter MMA */
        tkeylen = EVP_CIPHER_CTX_key_length(evp_ctx);
        tkey = OPENSSL_malloc(tkeylen);
        if (!tkey)
            goto err;
        if (EVP_CIPHER_CTX_rand_key(evp_ctx, tkey) <= 0)
            goto err;
        /* If we have no key use random key */
        if (jj <= 0) {
            OPENSSL_free(tmp);
            jj = tkeylen;
            tmp = tkey;
            tkey = NULL;
        }

        if (jj != tkeylen) {
            /*
             * Some S/MIME clients don't use the same key and effective key
             * length. The key length is determined by the size of the
             * decrypted RSA key.
             */
            if (!EVP_CIPHER_CTX_set_key_length(evp_ctx, jj)) {
                /* As MMA defence use random key instead */
                OPENSSL_cleanse(tmp, jj);
                OPENSSL_free(tmp);
                jj = tkeylen;
                tmp = tkey;
                tkey = NULL;
            }
        }
        ERR_clear_error();
        if (EVP_CipherInit_ex(evp_ctx, NULL, NULL, tmp, NULL, 0) <= 0)
            goto err;

        OPENSSL_cleanse(tmp, jj);

        if (tkey) {
            OPENSSL_cleanse(tkey, tkeylen);
            OPENSSL_free(tkey);
        }

        if (out == NULL)
            out = etmp;
        else
            BIO_push(out, etmp);
        etmp = NULL;
    }
#if 1
    if (in_bio != NULL) {
        bio = in_bio;
    } else {
# if 0
        bio = BIO_new(BIO_s_mem());
        /*
         * We need to set this so that when we have read all the data, the
         * encrypt BIO, if present, will read EOF and encode the last few
         * bytes
         */
        BIO_set_mem_eof_return(bio, 0);

        if (data_body->length > 0)
            BIO_write(bio, (char *)data_body->data, data_body->length);
# else
        if (data_body->length > 0)
            bio = BIO_new_mem_buf(data_body->data, data_body->length);
        else {
            bio = BIO_new(BIO_s_mem());
            BIO_set_mem_eof_return(bio, 0);
        }
        if (bio == NULL)
            goto err;
# endif
    }
    BIO_push(out, bio);
    bio = NULL;
#endif
    if (0) {
 err:
        if (out != NULL)
            BIO_free_all(out);
        if (btmp != NULL)
            BIO_free_all(btmp);
        if (etmp != NULL)
            BIO_free_all(etmp);
        if (bio != NULL)
            BIO_free_all(bio);
        out = NULL;
    }
    if (tmp != NULL)
        OPENSSL_free(tmp);
    return (out);
}