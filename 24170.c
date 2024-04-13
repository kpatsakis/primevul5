const BIGNUM *ECDSA_SIG_get0_s(const ECDSA_SIG *sig)
{
    return sig->s;
}