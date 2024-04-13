int i2d_ECPrivateKey(EC_KEY *a, unsigned char **out)
{
    int ret = 0, ok = 0;
    unsigned char *priv= NULL, *pub= NULL;
    size_t privlen = 0, publen = 0;

    EC_PRIVATEKEY *priv_key = NULL;

    if (a == NULL || a->group == NULL ||
        (!(a->enc_flag & EC_PKEY_NO_PUBKEY) && a->pub_key == NULL)) {
        ECerr(EC_F_I2D_ECPRIVATEKEY, ERR_R_PASSED_NULL_PARAMETER);
        goto err;
    }

    if ((priv_key = EC_PRIVATEKEY_new()) == NULL) {
        ECerr(EC_F_I2D_ECPRIVATEKEY, ERR_R_MALLOC_FAILURE);
        goto err;
    }

    priv_key->version = a->version;

    privlen = EC_KEY_priv2buf(a, &priv);

    if (privlen == 0) {
        ECerr(EC_F_I2D_ECPRIVATEKEY, ERR_R_EC_LIB);
        goto err;
    }

    ASN1_STRING_set0(priv_key->privateKey, priv, privlen);
    priv = NULL;

    if (!(a->enc_flag & EC_PKEY_NO_PARAMETERS)) {
        if ((priv_key->parameters =
             EC_GROUP_get_ecpkparameters(a->group,
                                        priv_key->parameters)) == NULL) {
            ECerr(EC_F_I2D_ECPRIVATEKEY, ERR_R_EC_LIB);
            goto err;
        }
    }

    if (!(a->enc_flag & EC_PKEY_NO_PUBKEY)) {
        priv_key->publicKey = ASN1_BIT_STRING_new();
        if (priv_key->publicKey == NULL) {
            ECerr(EC_F_I2D_ECPRIVATEKEY, ERR_R_MALLOC_FAILURE);
            goto err;
        }

        publen = EC_KEY_key2buf(a, a->conv_form, &pub, NULL);

        if (publen == 0) {
            ECerr(EC_F_I2D_ECPRIVATEKEY, ERR_R_EC_LIB);
            goto err;
        }

        priv_key->publicKey->flags &= ~(ASN1_STRING_FLAG_BITS_LEFT | 0x07);
        priv_key->publicKey->flags |= ASN1_STRING_FLAG_BITS_LEFT;
        ASN1_STRING_set0(priv_key->publicKey, pub, publen);
        pub = NULL;
    }

    if ((ret = i2d_EC_PRIVATEKEY(priv_key, out)) == 0) {
        ECerr(EC_F_I2D_ECPRIVATEKEY, ERR_R_EC_LIB);
        goto err;
    }
    ok = 1;
 err:
    OPENSSL_clear_free(priv, privlen);
    OPENSSL_free(pub);
    EC_PRIVATEKEY_free(priv_key);
    return (ok ? ret : 0);
}