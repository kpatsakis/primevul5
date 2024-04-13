void evp_final(EVPCTX ctx, unsigned char *md, unsigned int *mdlen)
{
    EVP_DigestFinal(ctx, md, mdlen);
}