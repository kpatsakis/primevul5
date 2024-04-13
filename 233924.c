void evp_update(EVPCTX ctx, const void *data, unsigned long len)
{
    EVP_DigestUpdate(ctx, data, len);
}