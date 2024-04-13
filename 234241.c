static int ecdsa_sign_setup(EC_KEY *eckey, BN_CTX *ctx_in,
                            BIGNUM **kinvp, BIGNUM **rp,
                            const unsigned char *dgst, int dlen)
{
    BN_CTX *ctx = NULL;
    BIGNUM *k = NULL, *r = NULL, *X = NULL;
    const BIGNUM *order;
    EC_POINT *tmp_point = NULL;
    const EC_GROUP *group;
    int ret = 0;
    int order_bits;

    if (eckey == NULL || (group = EC_KEY_get0_group(eckey)) == NULL) {
        ECerr(EC_F_ECDSA_SIGN_SETUP, ERR_R_PASSED_NULL_PARAMETER);
        return 0;
    }

    if (!EC_KEY_can_sign(eckey)) {
        ECerr(EC_F_ECDSA_SIGN_SETUP, EC_R_CURVE_DOES_NOT_SUPPORT_SIGNING);
        return 0;
    }

    if (ctx_in == NULL) {
        if ((ctx = BN_CTX_new()) == NULL) {
            ECerr(EC_F_ECDSA_SIGN_SETUP, ERR_R_MALLOC_FAILURE);
            return 0;
        }
    } else
        ctx = ctx_in;

    k = BN_new();               /* this value is later returned in *kinvp */
    r = BN_new();               /* this value is later returned in *rp */
    X = BN_new();
    if (k == NULL || r == NULL || X == NULL) {
        ECerr(EC_F_ECDSA_SIGN_SETUP, ERR_R_MALLOC_FAILURE);
        goto err;
    }
    if ((tmp_point = EC_POINT_new(group)) == NULL) {
        ECerr(EC_F_ECDSA_SIGN_SETUP, ERR_R_EC_LIB);
        goto err;
    }
    order = EC_GROUP_get0_order(group);
    if (order == NULL) {
        ECerr(EC_F_ECDSA_SIGN_SETUP, ERR_R_EC_LIB);
        goto err;
    }

    /* Preallocate space */
    order_bits = BN_num_bits(order);
    if (!BN_set_bit(k, order_bits)
        || !BN_set_bit(r, order_bits)
        || !BN_set_bit(X, order_bits))
        goto err;

    do {
        /* get random k */
        do
            if (dgst != NULL) {
                if (!BN_generate_dsa_nonce
                    (k, order, EC_KEY_get0_private_key(eckey), dgst, dlen,
                     ctx)) {
                    ECerr(EC_F_ECDSA_SIGN_SETUP,
                             EC_R_RANDOM_NUMBER_GENERATION_FAILED);
                    goto err;
                }
            } else {
                if (!BN_rand_range(k, order)) {
                    ECerr(EC_F_ECDSA_SIGN_SETUP,
                             EC_R_RANDOM_NUMBER_GENERATION_FAILED);
                    goto err;
                }
            }
        while (BN_is_zero(k));

        /*
         * We do not want timing information to leak the length of k, so we
         * compute G*k using an equivalent scalar of fixed bit-length.
         *
         * We unconditionally perform both of these additions to prevent a
         * small timing information leakage.  We then choose the sum that is
         * one bit longer than the order.  This guarantees the code
         * path used in the constant time implementations elsewhere.
         *
         * TODO: revisit the BN_copy aiming for a memory access agnostic
         * conditional copy.
         */
        if (!BN_add(r, k, order)
            || !BN_add(X, r, order)
            || !BN_copy(k, BN_num_bits(r) > order_bits ? r : X))
            goto err;

        /* compute r the x-coordinate of generator * k */
        if (!EC_POINT_mul(group, tmp_point, k, NULL, NULL, ctx)) {
            ECerr(EC_F_ECDSA_SIGN_SETUP, ERR_R_EC_LIB);
            goto err;
        }
        if (EC_METHOD_get_field_type(EC_GROUP_method_of(group)) ==
            NID_X9_62_prime_field) {
            if (!EC_POINT_get_affine_coordinates_GFp
                (group, tmp_point, X, NULL, ctx)) {
                ECerr(EC_F_ECDSA_SIGN_SETUP, ERR_R_EC_LIB);
                goto err;
            }
        }
#ifndef OPENSSL_NO_EC2M
        else {                  /* NID_X9_62_characteristic_two_field */

            if (!EC_POINT_get_affine_coordinates_GF2m(group,
                                                      tmp_point, X, NULL,
                                                      ctx)) {
                ECerr(EC_F_ECDSA_SIGN_SETUP, ERR_R_EC_LIB);
                goto err;
            }
        }
#endif
        if (!BN_nnmod(r, X, order, ctx)) {
            ECerr(EC_F_ECDSA_SIGN_SETUP, ERR_R_BN_LIB);
            goto err;
        }
    }
    while (BN_is_zero(r));

    /* compute the inverse of k */
    if (EC_GROUP_get_mont_data(group) != NULL) {
        /*
         * We want inverse in constant time, therefore we utilize the fact
         * order must be prime and use Fermats Little Theorem instead.
         */
        if (!BN_set_word(X, 2)) {
            ECerr(EC_F_ECDSA_SIGN_SETUP, ERR_R_BN_LIB);
            goto err;
        }
        if (!BN_mod_sub(X, order, X, order, ctx)) {
            ECerr(EC_F_ECDSA_SIGN_SETUP, ERR_R_BN_LIB);
            goto err;
        }
        BN_set_flags(X, BN_FLG_CONSTTIME);
        if (!BN_mod_exp_mont_consttime
            (k, k, X, order, ctx, EC_GROUP_get_mont_data(group))) {
            ECerr(EC_F_ECDSA_SIGN_SETUP, ERR_R_BN_LIB);
            goto err;
        }
    } else {
        if (!BN_mod_inverse(k, k, order, ctx)) {
            ECerr(EC_F_ECDSA_SIGN_SETUP, ERR_R_BN_LIB);
            goto err;
        }
    }

    /* clear old values if necessary */
    BN_clear_free(*rp);
    BN_clear_free(*kinvp);
    /* save the pre-computed values  */
    *rp = r;
    *kinvp = k;
    ret = 1;
 err:
    if (!ret) {
        BN_clear_free(k);
        BN_clear_free(r);
    }
    if (ctx != ctx_in)
        BN_CTX_free(ctx);
    EC_POINT_free(tmp_point);
    BN_clear_free(X);
    return (ret);
}