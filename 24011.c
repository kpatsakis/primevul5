void *SSL_SESSION_get_ex_data(const SSL_SESSION *s, int idx)
{
    return (CRYPTO_get_ex_data(&s->ex_data, idx));
}