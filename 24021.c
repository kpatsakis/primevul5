int SSL_SESSION_set_ex_data(SSL_SESSION *s, int idx, void *arg)
{
    return (CRYPTO_set_ex_data(&s->ex_data, idx, arg));
}