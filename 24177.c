EC_GROUP *EC_GROUP_new_from_ecpkparameters(const ECPKPARAMETERS *params)
{
    EC_GROUP *ret = NULL;
    int tmp = 0;

    if (params == NULL) {
        ECerr(EC_F_EC_GROUP_NEW_FROM_ECPKPARAMETERS, EC_R_MISSING_PARAMETERS);
        return NULL;
    }

    if (params->type == ECPKPARAMETERS_TYPE_NAMED) {
        /* the curve is given by an OID */
        tmp = OBJ_obj2nid(params->value.named_curve);
        if ((ret = EC_GROUP_new_by_curve_name(tmp)) == NULL) {
            ECerr(EC_F_EC_GROUP_NEW_FROM_ECPKPARAMETERS,
                  EC_R_EC_GROUP_NEW_BY_NAME_FAILURE);
            return NULL;
        }
        EC_GROUP_set_asn1_flag(ret, OPENSSL_EC_NAMED_CURVE);
    } else if (params->type == ECPKPARAMETERS_TYPE_EXPLICIT) {
        /* the parameters are given by an ECPARAMETERS structure */
        ret = EC_GROUP_new_from_ecparameters(params->value.parameters);
        if (!ret) {
            ECerr(EC_F_EC_GROUP_NEW_FROM_ECPKPARAMETERS, ERR_R_EC_LIB);
            return NULL;
        }
        EC_GROUP_set_asn1_flag(ret, OPENSSL_EC_EXPLICIT_CURVE);
    } else if (params->type == ECPKPARAMETERS_TYPE_IMPLICIT) {
        /* implicit parameters inherited from CA - unsupported */
        return NULL;
    } else {
        ECerr(EC_F_EC_GROUP_NEW_FROM_ECPKPARAMETERS, EC_R_ASN1_ERROR);
        return NULL;
    }

    return ret;
}