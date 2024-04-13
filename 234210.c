const char *SSL_rstate_string(const SSL *s)
{
    switch (s->rlayer.rstate) {
    case SSL_ST_READ_HEADER:
        return "RH";
    case SSL_ST_READ_BODY:
        return "RB";
    case SSL_ST_READ_DONE:
        return "RD";
    default:
        return "unknown";
    }
}