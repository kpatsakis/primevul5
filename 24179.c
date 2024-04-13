static int ec_asn1_group2curve(const EC_GROUP *group, X9_62_CURVE *curve)
{
    int ok = 0;
    BIGNUM *tmp_1 = NULL, *tmp_2 = NULL;
    unsigned char *a_buf = NULL, *b_buf = NULL;
    size_t len;

    if (!group || !curve || !curve->a || !curve->b)
        return 0;

    if ((tmp_1 = BN_new()) == NULL || (tmp_2 = BN_new()) == NULL) {
        ECerr(EC_F_EC_ASN1_GROUP2CURVE, ERR_R_MALLOC_FAILURE);
        goto err;
    }

    /* get a and b */
    if (!EC_GROUP_get_curve(group, NULL, tmp_1, tmp_2, NULL)) {
        ECerr(EC_F_EC_ASN1_GROUP2CURVE, ERR_R_EC_LIB);
        goto err;
    }

    /*
     * Per SEC 1, the curve coefficients must be padded up to size. See C.2's
     * definition of Curve, C.1's definition of FieldElement, and 2.3.5's
     * definition of how to encode the field elements.
     */
    len = ((size_t)EC_GROUP_get_degree(group) + 7) / 8;
    if ((a_buf = OPENSSL_malloc(len)) == NULL
        || (b_buf = OPENSSL_malloc(len)) == NULL) {
        ECerr(EC_F_EC_ASN1_GROUP2CURVE, ERR_R_MALLOC_FAILURE);
        goto err;
    }
    if (BN_bn2binpad(tmp_1, a_buf, len) < 0
        || BN_bn2binpad(tmp_2, b_buf, len) < 0) {
        ECerr(EC_F_EC_ASN1_GROUP2CURVE, ERR_R_BN_LIB);
        goto err;
    }

    /* set a and b */
    if (!ASN1_OCTET_STRING_set(curve->a, a_buf, len)
        || !ASN1_OCTET_STRING_set(curve->b, b_buf, len)) {
        ECerr(EC_F_EC_ASN1_GROUP2CURVE, ERR_R_ASN1_LIB);
        goto err;
    }

    /* set the seed (optional) */
    if (group->seed) {
        if (!curve->seed)
            if ((curve->seed = ASN1_BIT_STRING_new()) == NULL) {
                ECerr(EC_F_EC_ASN1_GROUP2CURVE, ERR_R_MALLOC_FAILURE);
                goto err;
            }
        curve->seed->flags &= ~(ASN1_STRING_FLAG_BITS_LEFT | 0x07);
        curve->seed->flags |= ASN1_STRING_FLAG_BITS_LEFT;
        if (!ASN1_BIT_STRING_set(curve->seed, group->seed,
                                 (int)group->seed_len)) {
            ECerr(EC_F_EC_ASN1_GROUP2CURVE, ERR_R_ASN1_LIB);
            goto err;
        }
    } else {
        ASN1_BIT_STRING_free(curve->seed);
        curve->seed = NULL;
    }

    ok = 1;

 err:
    OPENSSL_free(a_buf);
    OPENSSL_free(b_buf);
    BN_free(tmp_1);
    BN_free(tmp_2);
    return ok;
}