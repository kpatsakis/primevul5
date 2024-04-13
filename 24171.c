ECPKPARAMETERS *EC_GROUP_get_ecpkparameters(const EC_GROUP *group,
                                            ECPKPARAMETERS *params)
{
    int ok = 1, tmp;
    ECPKPARAMETERS *ret = params;

    if (ret == NULL) {
        if ((ret = ECPKPARAMETERS_new()) == NULL) {
            ECerr(EC_F_EC_GROUP_GET_ECPKPARAMETERS, ERR_R_MALLOC_FAILURE);
            return NULL;
        }
    } else {
        if (ret->type == ECPKPARAMETERS_TYPE_NAMED)
            ASN1_OBJECT_free(ret->value.named_curve);
        else if (ret->type == ECPKPARAMETERS_TYPE_EXPLICIT
                 && ret->value.parameters != NULL)
            ECPARAMETERS_free(ret->value.parameters);
    }

    if (EC_GROUP_get_asn1_flag(group)) {
        /*
         * use the asn1 OID to describe the elliptic curve parameters
         */
        tmp = EC_GROUP_get_curve_name(group);
        if (tmp) {
            ASN1_OBJECT *asn1obj = OBJ_nid2obj(tmp);

            if (asn1obj == NULL || OBJ_length(asn1obj) == 0) {
                ASN1_OBJECT_free(asn1obj);
                ECerr(EC_F_EC_GROUP_GET_ECPKPARAMETERS, EC_R_MISSING_OID);
                ok = 0;
            } else {
                ret->type = ECPKPARAMETERS_TYPE_NAMED;
                ret->value.named_curve = asn1obj;
            }
        } else
            /* we don't know the nid => ERROR */
            ok = 0;
    } else {
        /* use the ECPARAMETERS structure */
        ret->type = ECPKPARAMETERS_TYPE_EXPLICIT;
        if ((ret->value.parameters =
             EC_GROUP_get_ecparameters(group, NULL)) == NULL)
            ok = 0;
    }

    if (!ok) {
        ECPKPARAMETERS_free(ret);
        return NULL;
    }
    return ret;
}