ECDSA_SIG *ossl_ecdsa_sign_sig(const unsigned char *dgst, int dgst_len,
                               const BIGNUM *in_kinv, const BIGNUM *in_r,
                               EC_KEY *eckey)
{
    int ok = 0, i;
    BIGNUM *kinv = NULL, *s, *m = NULL, *tmp = NULL, *blind = NULL;
    BIGNUM *blindm = NULL;
    const BIGNUM *order, *ckinv;
    BN_CTX *ctx = NULL;
    const EC_GROUP *group;
    ECDSA_SIG *ret;
    const BIGNUM *priv_key;

    group = EC_KEY_get0_group(eckey);
    priv_key = EC_KEY_get0_private_key(eckey);

    if (group == NULL || priv_key == NULL) {
        ECerr(EC_F_OSSL_ECDSA_SIGN_SIG, ERR_R_PASSED_NULL_PARAMETER);
        return NULL;
    }

    if (!EC_KEY_can_sign(eckey)) {
        ECerr(EC_F_OSSL_ECDSA_SIGN_SIG, EC_R_CURVE_DOES_NOT_SUPPORT_SIGNING);
        return NULL;
    }

    ret = ECDSA_SIG_new();
    if (ret == NULL) {
        ECerr(EC_F_OSSL_ECDSA_SIGN_SIG, ERR_R_MALLOC_FAILURE);
        return NULL;
    }
    ret->r = BN_new();
    ret->s = BN_new();
    if (ret->r == NULL || ret->s == NULL) {
        ECerr(EC_F_OSSL_ECDSA_SIGN_SIG, ERR_R_MALLOC_FAILURE);
        goto err;
    }
    s = ret->s;

    ctx = BN_CTX_secure_new();
    if (ctx == NULL) {
        ECerr(EC_F_OSSL_ECDSA_SIGN_SIG, ERR_R_MALLOC_FAILURE);
        goto err;
    }

    BN_CTX_start(ctx);
    tmp = BN_CTX_get(ctx);
    m = BN_CTX_get(ctx);
    blind = BN_CTX_get(ctx);
    blindm = BN_CTX_get(ctx);
    if (blindm == NULL) {
        ECerr(EC_F_OSSL_ECDSA_SIGN_SIG, ERR_R_MALLOC_FAILURE);
        goto err;
    }

    order = EC_GROUP_get0_order(group);
    if (order == NULL) {
        ECerr(EC_F_OSSL_ECDSA_SIGN_SIG, ERR_R_EC_LIB);
        goto err;
    }
    i = BN_num_bits(order);
    /*
     * Need to truncate digest if it is too long: first truncate whole bytes.
     */
    if (8 * dgst_len > i)
        dgst_len = (i + 7) / 8;
    if (!BN_bin2bn(dgst, dgst_len, m)) {
        ECerr(EC_F_OSSL_ECDSA_SIGN_SIG, ERR_R_BN_LIB);
        goto err;
    }
    /* If still too long truncate remaining bits with a shift */
    if ((8 * dgst_len > i) && !BN_rshift(m, m, 8 - (i & 0x7))) {
        ECerr(EC_F_OSSL_ECDSA_SIGN_SIG, ERR_R_BN_LIB);
        goto err;
    }
    do {
        if (in_kinv == NULL || in_r == NULL) {
            if (!ecdsa_sign_setup(eckey, ctx, &kinv, &ret->r, dgst, dgst_len)) {
                ECerr(EC_F_OSSL_ECDSA_SIGN_SIG, ERR_R_ECDSA_LIB);
                goto err;
            }
            ckinv = kinv;
        } else {
            ckinv = in_kinv;
            if (BN_copy(ret->r, in_r) == NULL) {
                ECerr(EC_F_OSSL_ECDSA_SIGN_SIG, ERR_R_MALLOC_FAILURE);
                goto err;
            }
        }

        /*
         * The normal signature calculation is:
         *
         *   s := k^-1 * (m + r * priv_key) mod order
         *
         * We will blind this to protect against side channel attacks
         *
         *   s := k^-1 * blind^-1 * (blind * m + blind * r * priv_key) mod order
         */

        /* Generate a blinding value */
        do {
            if (!BN_rand(blind, BN_num_bits(order) - 1, BN_RAND_TOP_ANY,
                         BN_RAND_BOTTOM_ANY))
                goto err;
        } while (BN_is_zero(blind));
        BN_set_flags(blind, BN_FLG_CONSTTIME);
        BN_set_flags(blindm, BN_FLG_CONSTTIME);
        BN_set_flags(tmp, BN_FLG_CONSTTIME);

        /* tmp := blind * priv_key * r mod order */
        if (!BN_mod_mul(tmp, blind, priv_key, order, ctx)) {
            ECerr(EC_F_OSSL_ECDSA_SIGN_SIG, ERR_R_BN_LIB);
            goto err;
        }
        if (!BN_mod_mul(tmp, tmp, ret->r, order, ctx)) {
            ECerr(EC_F_OSSL_ECDSA_SIGN_SIG, ERR_R_BN_LIB);
            goto err;
        }

        /* blindm := blind * m mod order */
        if (!BN_mod_mul(blindm, blind, m, order, ctx)) {
            ECerr(EC_F_OSSL_ECDSA_SIGN_SIG, ERR_R_BN_LIB);
            goto err;
        }

        /* s : = (blind * priv_key * r) + (blind * m) mod order */
        if (!BN_mod_add_quick(s, tmp, blindm, order)) {
            ECerr(EC_F_OSSL_ECDSA_SIGN_SIG, ERR_R_BN_LIB);
            goto err;
        }

        /* s:= s * blind^-1 mod order */
        if (BN_mod_inverse(blind, blind, order, ctx) == NULL) {
            ECerr(EC_F_OSSL_ECDSA_SIGN_SIG, ERR_R_BN_LIB);
            goto err;
        }
        if (!BN_mod_mul(s, s, blind, order, ctx)) {
            ECerr(EC_F_OSSL_ECDSA_SIGN_SIG, ERR_R_BN_LIB);
            goto err;
        }

        /* s := s * k^-1 mod order */
        if (!BN_mod_mul(s, s, ckinv, order, ctx)) {
            ECerr(EC_F_OSSL_ECDSA_SIGN_SIG, ERR_R_BN_LIB);
            goto err;
        }

        if (BN_is_zero(s)) {
            /*
             * if kinv and r have been supplied by the caller don't to
             * generate new kinv and r values
             */
            if (in_kinv != NULL && in_r != NULL) {
                ECerr(EC_F_OSSL_ECDSA_SIGN_SIG, EC_R_NEED_NEW_SETUP_VALUES);
                goto err;
            }
        } else
            /* s != 0 => we have a valid signature */
            break;
    }
    while (1);

    ok = 1;
 err:
    if (!ok) {
        ECDSA_SIG_free(ret);
        ret = NULL;
    }
    BN_CTX_end(ctx);
    BN_CTX_free(ctx);
    BN_clear_free(kinv);
    return ret;
}