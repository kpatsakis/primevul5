long SSL_SESSION_get_time(const SSL_SESSION *s)
{
    if (s == NULL)
        return (0);
    return (s->time);
}