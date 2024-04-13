int SSL_SESSION_get_ex_new_index(long argl, void *argp,
                                 CRYPTO_EX_new *new_func,
                                 CRYPTO_EX_dup *dup_func,
                                 CRYPTO_EX_free *free_func)
{
    return CRYPTO_get_ex_new_index(CRYPTO_EX_INDEX_SSL_SESSION, argl, argp,
                                   new_func, dup_func, free_func);
}