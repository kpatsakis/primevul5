const BIGNUM *ECDSA_SIG_get0_r(const ECDSA_SIG *sig)
{
    return sig->r;
}