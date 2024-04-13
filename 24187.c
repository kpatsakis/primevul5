ECPARAMETERS *EC_GROUP_get_ecparameters(const EC_GROUP *group,
                                               ECPARAMETERS *params)
{
    size_t len = 0;
    ECPARAMETERS *ret = NULL;
    const BIGNUM *tmp;
    unsigned char *buffer = NULL;
    const EC_POINT *point = NULL;
    point_conversion_form_t form;
    ASN1_INTEGER *orig;

    if (params == NULL) {
        if ((ret = ECPARAMETERS_new()) == NULL) {
            ECerr(EC_F_EC_GROUP_GET_ECPARAMETERS, ERR_R_MALLOC_FAILURE);
            goto err;
        }
    } else
        ret = params;

    /* set the version (always one) */
    ret->version = (long)0x1;

    /* set the fieldID */
    if (!ec_asn1_group2fieldid(group, ret->fieldID)) {
        ECerr(EC_F_EC_GROUP_GET_ECPARAMETERS, ERR_R_EC_LIB);
        goto err;
    }

    /* set the curve */
    if (!ec_asn1_group2curve(group, ret->curve)) {
        ECerr(EC_F_EC_GROUP_GET_ECPARAMETERS, ERR_R_EC_LIB);
        goto err;
    }

    /* set the base point */
    if ((point = EC_GROUP_get0_generator(group)) == NULL) {
        ECerr(EC_F_EC_GROUP_GET_ECPARAMETERS, EC_R_UNDEFINED_GENERATOR);
        goto err;
    }

    form = EC_GROUP_get_point_conversion_form(group);

    len = EC_POINT_point2buf(group, point, form, &buffer, NULL);
    if (len == 0) {
        ECerr(EC_F_EC_GROUP_GET_ECPARAMETERS, ERR_R_EC_LIB);
        goto err;
    }
    if (ret->base == NULL && (ret->base = ASN1_OCTET_STRING_new()) == NULL) {
        OPENSSL_free(buffer);
        ECerr(EC_F_EC_GROUP_GET_ECPARAMETERS, ERR_R_MALLOC_FAILURE);
        goto err;
    }
    ASN1_STRING_set0(ret->base, buffer, len);

    /* set the order */
    tmp = EC_GROUP_get0_order(group);
    if (tmp == NULL) {
        ECerr(EC_F_EC_GROUP_GET_ECPARAMETERS, ERR_R_EC_LIB);
        goto err;
    }
    ret->order = BN_to_ASN1_INTEGER(tmp, orig = ret->order);
    if (ret->order == NULL) {
        ret->order = orig;
        ECerr(EC_F_EC_GROUP_GET_ECPARAMETERS, ERR_R_ASN1_LIB);
        goto err;
    }

    /* set the cofactor (optional) */
    tmp = EC_GROUP_get0_cofactor(group);
    if (tmp != NULL) {
        ret->cofactor = BN_to_ASN1_INTEGER(tmp, orig = ret->cofactor);
        if (ret->cofactor == NULL) {
            ret->cofactor = orig;
            ECerr(EC_F_EC_GROUP_GET_ECPARAMETERS, ERR_R_ASN1_LIB);
            goto err;
        }
    }

    return ret;

 err:
    if (params == NULL)
        ECPARAMETERS_free(ret);
    return NULL;
}