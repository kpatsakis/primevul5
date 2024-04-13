static void * xar_hash_init(int hash, void **sc, void **mc)
{
    if (!sc && !mc)
        return NULL;
    switch (hash) {
    case XAR_CKSUM_SHA1:
        *sc = cl_hash_init("sha1");
        if (!(*sc)) {
            return NULL;
        }

        return *sc;
    case XAR_CKSUM_MD5:
        *mc = cl_hash_init("md5");
        if (!(*mc)) {
            return NULL;
        }

        return *mc;
    case XAR_CKSUM_OTHER:
    case XAR_CKSUM_NONE:
    default:
        return NULL;
    }
}