long SSL_CTX_get_timeout(const SSL_CTX *s)
{
    if (s == NULL)
        return (0);
    return (s->session_timeout);
}