int i2o_ECPublicKey(const EC_KEY *a, unsigned char **out)
{
    size_t buf_len = 0;
    int new_buffer = 0;

    if (a == NULL) {
        ECerr(EC_F_I2O_ECPUBLICKEY, ERR_R_PASSED_NULL_PARAMETER);
        return 0;
    }

    buf_len = EC_POINT_point2oct(a->group, a->pub_key,
                                 a->conv_form, NULL, 0, NULL);

    if (out == NULL || buf_len == 0)
        /* out == NULL => just return the length of the octet string */
        return buf_len;

    if (*out == NULL) {
        if ((*out = OPENSSL_malloc(buf_len)) == NULL) {
            ECerr(EC_F_I2O_ECPUBLICKEY, ERR_R_MALLOC_FAILURE);
            return 0;
        }
        new_buffer = 1;
    }
    if (!EC_POINT_point2oct(a->group, a->pub_key, a->conv_form,
                            *out, buf_len, NULL)) {
        ECerr(EC_F_I2O_ECPUBLICKEY, ERR_R_EC_LIB);
        if (new_buffer) {
            OPENSSL_free(*out);
            *out = NULL;
        }
        return 0;
    }
    if (!new_buffer)
        *out += buf_len;
    return buf_len;
}