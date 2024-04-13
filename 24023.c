long SSL_CTX_set_timeout(SSL_CTX *s, long t)
{
    long l;
    if (s == NULL)
        return (0);
    l = s->session_timeout;
    s->session_timeout = t;
    return (l);
}