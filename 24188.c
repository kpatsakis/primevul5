ECDSA_SIG *ECDSA_SIG_new(void)
{
    ECDSA_SIG *sig = OPENSSL_zalloc(sizeof(*sig));
    if (sig == NULL)
        ECerr(EC_F_ECDSA_SIG_NEW, ERR_R_MALLOC_FAILURE);
    return sig;
}