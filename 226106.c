static void xar_hash_update(void * hash_ctx, void * data, unsigned long size, int hash)
{
    if (!hash_ctx || !data || !size)
        return;

    switch (hash) {
    case XAR_CKSUM_NONE:
    case XAR_CKSUM_OTHER:
        return;
    }

    cl_update_hash(hash_ctx, data, size);
}