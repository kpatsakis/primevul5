EC_GROUP *d2i_ECPKParameters(EC_GROUP **a, const unsigned char **in, long len)
{
    EC_GROUP *group = NULL;
    ECPKPARAMETERS *params = NULL;
    const unsigned char *p = *in;

    if ((params = d2i_ECPKPARAMETERS(NULL, &p, len)) == NULL) {
        ECerr(EC_F_D2I_ECPKPARAMETERS, EC_R_D2I_ECPKPARAMETERS_FAILURE);
        ECPKPARAMETERS_free(params);
        return NULL;
    }

    if ((group = EC_GROUP_new_from_ecpkparameters(params)) == NULL) {
        ECerr(EC_F_D2I_ECPKPARAMETERS, EC_R_PKPARAMETERS2GROUP_FAILURE);
        ECPKPARAMETERS_free(params);
        return NULL;
    }

    if (params->type == ECPKPARAMETERS_TYPE_EXPLICIT)
        group->decoded_from_explicit_params = 1;

    if (a) {
        EC_GROUP_free(*a);
        *a = group;
    }

    ECPKPARAMETERS_free(params);
    *in = p;
    return group;
}