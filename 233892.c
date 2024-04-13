EVPCTX evp_init(int nid)
{
    const EVP_MD *evp_md = nid_to_evpmd(nid);

    EVPCTX ctx = malloc(sizeof(EVP_MD_CTX));
    if (ctx == NULL) {
        return NULL;
    }

    EVP_DigestInit(ctx, evp_md);

    return ctx;
}