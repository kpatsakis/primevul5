EC_GROUP *EC_GROUP_new_from_ecparameters(const ECPARAMETERS *params)
{
    int ok = 0, tmp;
    EC_GROUP *ret = NULL, *dup = NULL;
    BIGNUM *p = NULL, *a = NULL, *b = NULL;
    EC_POINT *point = NULL;
    long field_bits;
    int curve_name = NID_undef;
    BN_CTX *ctx = NULL;

    if (!params->fieldID || !params->fieldID->fieldType ||
        !params->fieldID->p.ptr) {
        ECerr(EC_F_EC_GROUP_NEW_FROM_ECPARAMETERS, EC_R_ASN1_ERROR);
        goto err;
    }

    /*
     * Now extract the curve parameters a and b. Note that, although SEC 1
     * specifies the length of their encodings, historical versions of OpenSSL
     * encoded them incorrectly, so we must accept any length for backwards
     * compatibility.
     */
    if (!params->curve || !params->curve->a ||
        !params->curve->a->data || !params->curve->b ||
        !params->curve->b->data) {
        ECerr(EC_F_EC_GROUP_NEW_FROM_ECPARAMETERS, EC_R_ASN1_ERROR);
        goto err;
    }
    a = BN_bin2bn(params->curve->a->data, params->curve->a->length, NULL);
    if (a == NULL) {
        ECerr(EC_F_EC_GROUP_NEW_FROM_ECPARAMETERS, ERR_R_BN_LIB);
        goto err;
    }
    b = BN_bin2bn(params->curve->b->data, params->curve->b->length, NULL);
    if (b == NULL) {
        ECerr(EC_F_EC_GROUP_NEW_FROM_ECPARAMETERS, ERR_R_BN_LIB);
        goto err;
    }

    /* get the field parameters */
    tmp = OBJ_obj2nid(params->fieldID->fieldType);
    if (tmp == NID_X9_62_characteristic_two_field)
#ifdef OPENSSL_NO_EC2M
    {
        ECerr(EC_F_EC_GROUP_NEW_FROM_ECPARAMETERS, EC_R_GF2M_NOT_SUPPORTED);
        goto err;
    }
#else
    {
        X9_62_CHARACTERISTIC_TWO *char_two;

        char_two = params->fieldID->p.char_two;

        field_bits = char_two->m;
        if (field_bits > OPENSSL_ECC_MAX_FIELD_BITS) {
            ECerr(EC_F_EC_GROUP_NEW_FROM_ECPARAMETERS, EC_R_FIELD_TOO_LARGE);
            goto err;
        }

        if ((p = BN_new()) == NULL) {
            ECerr(EC_F_EC_GROUP_NEW_FROM_ECPARAMETERS, ERR_R_MALLOC_FAILURE);
            goto err;
        }

        /* get the base type */
        tmp = OBJ_obj2nid(char_two->type);

        if (tmp == NID_X9_62_tpBasis) {
            long tmp_long;

            if (!char_two->p.tpBasis) {
                ECerr(EC_F_EC_GROUP_NEW_FROM_ECPARAMETERS, EC_R_ASN1_ERROR);
                goto err;
            }

            tmp_long = ASN1_INTEGER_get(char_two->p.tpBasis);

            if (!(char_two->m > tmp_long && tmp_long > 0)) {
                ECerr(EC_F_EC_GROUP_NEW_FROM_ECPARAMETERS,
                      EC_R_INVALID_TRINOMIAL_BASIS);
                goto err;
            }

            /* create the polynomial */
            if (!BN_set_bit(p, (int)char_two->m))
                goto err;
            if (!BN_set_bit(p, (int)tmp_long))
                goto err;
            if (!BN_set_bit(p, 0))
                goto err;
        } else if (tmp == NID_X9_62_ppBasis) {
            X9_62_PENTANOMIAL *penta;

            penta = char_two->p.ppBasis;
            if (!penta) {
                ECerr(EC_F_EC_GROUP_NEW_FROM_ECPARAMETERS, EC_R_ASN1_ERROR);
                goto err;
            }

            if (!
                (char_two->m > penta->k3 && penta->k3 > penta->k2
                 && penta->k2 > penta->k1 && penta->k1 > 0)) {
                ECerr(EC_F_EC_GROUP_NEW_FROM_ECPARAMETERS,
                      EC_R_INVALID_PENTANOMIAL_BASIS);
                goto err;
            }

            /* create the polynomial */
            if (!BN_set_bit(p, (int)char_two->m))
                goto err;
            if (!BN_set_bit(p, (int)penta->k1))
                goto err;
            if (!BN_set_bit(p, (int)penta->k2))
                goto err;
            if (!BN_set_bit(p, (int)penta->k3))
                goto err;
            if (!BN_set_bit(p, 0))
                goto err;
        } else if (tmp == NID_X9_62_onBasis) {
            ECerr(EC_F_EC_GROUP_NEW_FROM_ECPARAMETERS, EC_R_NOT_IMPLEMENTED);
            goto err;
        } else {                /* error */

            ECerr(EC_F_EC_GROUP_NEW_FROM_ECPARAMETERS, EC_R_ASN1_ERROR);
            goto err;
        }

        /* create the EC_GROUP structure */
        ret = EC_GROUP_new_curve_GF2m(p, a, b, NULL);
    }
#endif
    else if (tmp == NID_X9_62_prime_field) {
        /* we have a curve over a prime field */
        /* extract the prime number */
        if (!params->fieldID->p.prime) {
            ECerr(EC_F_EC_GROUP_NEW_FROM_ECPARAMETERS, EC_R_ASN1_ERROR);
            goto err;
        }
        p = ASN1_INTEGER_to_BN(params->fieldID->p.prime, NULL);
        if (p == NULL) {
            ECerr(EC_F_EC_GROUP_NEW_FROM_ECPARAMETERS, ERR_R_ASN1_LIB);
            goto err;
        }

        if (BN_is_negative(p) || BN_is_zero(p)) {
            ECerr(EC_F_EC_GROUP_NEW_FROM_ECPARAMETERS, EC_R_INVALID_FIELD);
            goto err;
        }

        field_bits = BN_num_bits(p);
        if (field_bits > OPENSSL_ECC_MAX_FIELD_BITS) {
            ECerr(EC_F_EC_GROUP_NEW_FROM_ECPARAMETERS, EC_R_FIELD_TOO_LARGE);
            goto err;
        }

        /* create the EC_GROUP structure */
        ret = EC_GROUP_new_curve_GFp(p, a, b, NULL);
    } else {
        ECerr(EC_F_EC_GROUP_NEW_FROM_ECPARAMETERS, EC_R_INVALID_FIELD);
        goto err;
    }

    if (ret == NULL) {
        ECerr(EC_F_EC_GROUP_NEW_FROM_ECPARAMETERS, ERR_R_EC_LIB);
        goto err;
    }

    /* extract seed (optional) */
    if (params->curve->seed != NULL) {
        OPENSSL_free(ret->seed);
        if ((ret->seed = OPENSSL_malloc(params->curve->seed->length)) == NULL) {
            ECerr(EC_F_EC_GROUP_NEW_FROM_ECPARAMETERS, ERR_R_MALLOC_FAILURE);
            goto err;
        }
        memcpy(ret->seed, params->curve->seed->data,
               params->curve->seed->length);
        ret->seed_len = params->curve->seed->length;
    }

    if (params->order == NULL
            || params->base == NULL
            || params->base->data == NULL
            || params->base->length == 0) {
        ECerr(EC_F_EC_GROUP_NEW_FROM_ECPARAMETERS, EC_R_ASN1_ERROR);
        goto err;
    }

    if ((point = EC_POINT_new(ret)) == NULL)
        goto err;

    /* set the point conversion form */
    EC_GROUP_set_point_conversion_form(ret, (point_conversion_form_t)
                                       (params->base->data[0] & ~0x01));

    /* extract the ec point */
    if (!EC_POINT_oct2point(ret, point, params->base->data,
                            params->base->length, NULL)) {
        ECerr(EC_F_EC_GROUP_NEW_FROM_ECPARAMETERS, ERR_R_EC_LIB);
        goto err;
    }

    /* extract the order */
    if ((a = ASN1_INTEGER_to_BN(params->order, a)) == NULL) {
        ECerr(EC_F_EC_GROUP_NEW_FROM_ECPARAMETERS, ERR_R_ASN1_LIB);
        goto err;
    }
    if (BN_is_negative(a) || BN_is_zero(a)) {
        ECerr(EC_F_EC_GROUP_NEW_FROM_ECPARAMETERS, EC_R_INVALID_GROUP_ORDER);
        goto err;
    }
    if (BN_num_bits(a) > (int)field_bits + 1) { /* Hasse bound */
        ECerr(EC_F_EC_GROUP_NEW_FROM_ECPARAMETERS, EC_R_INVALID_GROUP_ORDER);
        goto err;
    }

    /* extract the cofactor (optional) */
    if (params->cofactor == NULL) {
        BN_free(b);
        b = NULL;
    } else if ((b = ASN1_INTEGER_to_BN(params->cofactor, b)) == NULL) {
        ECerr(EC_F_EC_GROUP_NEW_FROM_ECPARAMETERS, ERR_R_ASN1_LIB);
        goto err;
    }
    /* set the generator, order and cofactor (if present) */
    if (!EC_GROUP_set_generator(ret, point, a, b)) {
        ECerr(EC_F_EC_GROUP_NEW_FROM_ECPARAMETERS, ERR_R_EC_LIB);
        goto err;
    }

    /*
     * Check if the explicit parameters group just created matches one of the
     * built-in curves.
     *
     * We create a copy of the group just built, so that we can remove optional
     * fields for the lookup: we do this to avoid the possibility that one of
     * the optional parameters is used to force the library into using a less
     * performant and less secure EC_METHOD instead of the specialized one.
     * In any case, `seed` is not really used in any computation, while a
     * cofactor different from the one in the built-in table is just
     * mathematically wrong anyway and should not be used.
     */
    if ((ctx = BN_CTX_new()) == NULL) {
        ECerr(EC_F_EC_GROUP_NEW_FROM_ECPARAMETERS, ERR_R_BN_LIB);
        goto err;
    }
    if ((dup = EC_GROUP_dup(ret)) == NULL
            || EC_GROUP_set_seed(dup, NULL, 0) != 1
            || !EC_GROUP_set_generator(dup, point, a, NULL)) {
        ECerr(EC_F_EC_GROUP_NEW_FROM_ECPARAMETERS, ERR_R_EC_LIB);
        goto err;
    }
    if ((curve_name = ec_curve_nid_from_params(dup, ctx)) != NID_undef) {
        /*
         * The input explicit parameters successfully matched one of the
         * built-in curves: often for built-in curves we have specialized
         * methods with better performance and hardening.
         *
         * In this case we replace the `EC_GROUP` created through explicit
         * parameters with one created from a named group.
         */
        EC_GROUP *named_group = NULL;

#ifndef OPENSSL_NO_EC_NISTP_64_GCC_128
        /*
         * NID_wap_wsg_idm_ecid_wtls12 and NID_secp224r1 are both aliases for
         * the same curve, we prefer the SECP nid when matching explicit
         * parameters as that is associated with a specialized EC_METHOD.
         */
        if (curve_name == NID_wap_wsg_idm_ecid_wtls12)
            curve_name = NID_secp224r1;
#endif /* !def(OPENSSL_NO_EC_NISTP_64_GCC_128) */

        if ((named_group = EC_GROUP_new_by_curve_name(curve_name)) == NULL) {
            ECerr(EC_F_EC_GROUP_NEW_FROM_ECPARAMETERS, ERR_R_EC_LIB);
            goto err;
        }
        EC_GROUP_free(ret);
        ret = named_group;

        /*
         * Set the flag so that EC_GROUPs created from explicit parameters are
         * serialized using explicit parameters by default.
         */
        EC_GROUP_set_asn1_flag(ret, OPENSSL_EC_EXPLICIT_CURVE);

        /*
         * If the input params do not contain the optional seed field we make
         * sure it is not added to the returned group.
         *
         * The seed field is not really used inside libcrypto anyway, and
         * adding it to parsed explicit parameter keys would alter their DER
         * encoding output (because of the extra field) which could impact
         * applications fingerprinting keys by their DER encoding.
         */
        if (params->curve->seed == NULL) {
            if (EC_GROUP_set_seed(ret, NULL, 0) != 1)
                goto err;
        }
    }

    ok = 1;

 err:
    if (!ok) {
        EC_GROUP_free(ret);
        ret = NULL;
    }
    EC_GROUP_free(dup);

    BN_free(p);
    BN_free(a);
    BN_free(b);
    EC_POINT_free(point);

    BN_CTX_free(ctx);

    return ret;
}