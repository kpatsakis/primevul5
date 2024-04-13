EC_KEY *d2i_ECPrivateKey(EC_KEY **a, const unsigned char **in, long len)
{
    EC_KEY *ret = NULL;
    EC_PRIVATEKEY *priv_key = NULL;
    const unsigned char *p = *in;

    if ((priv_key = d2i_EC_PRIVATEKEY(NULL, &p, len)) == NULL) {
        ECerr(EC_F_D2I_ECPRIVATEKEY, ERR_R_EC_LIB);
        return NULL;
    }

    if (a == NULL || *a == NULL) {
        if ((ret = EC_KEY_new()) == NULL) {
            ECerr(EC_F_D2I_ECPRIVATEKEY, ERR_R_MALLOC_FAILURE);
            goto err;
        }
    } else
        ret = *a;

    if (priv_key->parameters) {
        EC_GROUP_free(ret->group);
        ret->group = EC_GROUP_new_from_ecpkparameters(priv_key->parameters);
        if (ret->group != NULL
            && priv_key->parameters->type == ECPKPARAMETERS_TYPE_EXPLICIT)
            ret->group->decoded_from_explicit_params = 1;
    }

    if (ret->group == NULL) {
        ECerr(EC_F_D2I_ECPRIVATEKEY, ERR_R_EC_LIB);
        goto err;
    }

    ret->version = priv_key->version;

    if (priv_key->privateKey) {
        ASN1_OCTET_STRING *pkey = priv_key->privateKey;
        if (EC_KEY_oct2priv(ret, ASN1_STRING_get0_data(pkey),
                            ASN1_STRING_length(pkey)) == 0)
            goto err;
    } else {
        ECerr(EC_F_D2I_ECPRIVATEKEY, EC_R_MISSING_PRIVATE_KEY);
        goto err;
    }

    EC_POINT_clear_free(ret->pub_key);
    ret->pub_key = EC_POINT_new(ret->group);
    if (ret->pub_key == NULL) {
        ECerr(EC_F_D2I_ECPRIVATEKEY, ERR_R_EC_LIB);
        goto err;
    }

    if (priv_key->publicKey) {
        const unsigned char *pub_oct;
        int pub_oct_len;

        pub_oct = ASN1_STRING_get0_data(priv_key->publicKey);
        pub_oct_len = ASN1_STRING_length(priv_key->publicKey);
        if (!EC_KEY_oct2key(ret, pub_oct, pub_oct_len, NULL)) {
            ECerr(EC_F_D2I_ECPRIVATEKEY, ERR_R_EC_LIB);
            goto err;
        }
    } else {
        if (ret->group->meth->keygenpub == NULL
            || ret->group->meth->keygenpub(ret) == 0)
                goto err;
        /* Remember the original private-key-only encoding. */
        ret->enc_flag |= EC_PKEY_NO_PUBKEY;
    }

    if (a)
        *a = ret;
    EC_PRIVATEKEY_free(priv_key);
    *in = p;
    return ret;

 err:
    if (a == NULL || *a != ret)
        EC_KEY_free(ret);
    EC_PRIVATEKEY_free(priv_key);
    return NULL;
}