const unsigned char *SSL_SESSION_get_id(const SSL_SESSION *s,
                                        unsigned int *len)
{
    if (len)
        *len = s->session_id_length;
    return s->session_id;
}