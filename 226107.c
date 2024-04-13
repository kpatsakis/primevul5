static void xar_hash_final(void * hash_ctx, void * result, int hash)
{
    if (!hash_ctx || !result)
        return;

    switch (hash) {
    case XAR_CKSUM_OTHER:
    case XAR_CKSUM_NONE:
        return;
    }

    cl_finish_hash(hash_ctx, result);
}