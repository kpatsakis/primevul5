int i2d_ECPKParameters(const EC_GROUP *a, unsigned char **out)
{
    int ret = 0;
    ECPKPARAMETERS *tmp = EC_GROUP_get_ecpkparameters(a, NULL);
    if (tmp == NULL) {
        ECerr(EC_F_I2D_ECPKPARAMETERS, EC_R_GROUP2PKPARAMETERS_FAILURE);
        return 0;
    }
    if ((ret = i2d_ECPKPARAMETERS(tmp, out)) == 0) {
        ECerr(EC_F_I2D_ECPKPARAMETERS, EC_R_I2D_ECPKPARAMETERS_FAILURE);
        ECPKPARAMETERS_free(tmp);
        return 0;
    }
    ECPKPARAMETERS_free(tmp);
    return ret;
}