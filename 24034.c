long SSL_SESSION_set_timeout(SSL_SESSION *s, long t)
{
    if (s == NULL)
        return (0);
    s->timeout = t;
    return (1);
}