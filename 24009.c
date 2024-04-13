long SSL_SESSION_set_time(SSL_SESSION *s, long t)
{
    if (s == NULL)
        return (0);
    s->time = t;
    return (t);
}