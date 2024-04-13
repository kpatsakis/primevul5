crypt_pw_enc_by_hash(const char *pwd, int hash_algo)
{
    char salt[3];
    char *algo_salt = NULL;
    char *cry;
    char *enc = NULL;
    long v;
    static unsigned int seed = 0;
    struct crypt_data data;
    data.initialized = 0;

    if (seed == 0) {
        seed = (unsigned int)slapi_rand();
    }
    v = slapi_rand_r(&seed);

    salt[0] = itoa64[v & 0x3f];
    v >>= 6;
    salt[1] = itoa64[v & 0x3f];
    salt[2] = '\0';

    /* Prepare our salt based on the hashing algorithm */
    if (hash_algo == CRYPT_UNIX) {
        algo_salt = strdup(salt);
    } else if (hash_algo == CRYPT_MD5) {
        algo_salt = slapi_ch_smprintf("$1$%s", salt);
    } else if (hash_algo == CRYPT_SHA256) {
        algo_salt = slapi_ch_smprintf("$5$%s", salt);
    } else if (hash_algo == CRYPT_SHA512) {
        algo_salt = slapi_ch_smprintf("$6$%s", salt);
    } else {
        /* default to CRYPT_UNIX */
        algo_salt = strdup(salt);
    }

    cry = crypt_r(pwd, algo_salt, &data);
    if (cry != NULL) {
        enc = slapi_ch_smprintf("%c%s%c%s", PWD_HASH_PREFIX_START, CRYPT_SCHEME_NAME, PWD_HASH_PREFIX_END, cry);
    }
    slapi_ch_free_string(&algo_salt);

    return (enc);
}