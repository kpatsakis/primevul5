const char *SSL_rstate_string_long(const SSL *s)
{
    switch (s->rlayer.rstate) {
    case SSL_ST_READ_HEADER:
        return "read header";
    case SSL_ST_READ_BODY:
        return "read body";
    case SSL_ST_READ_DONE:
        return "read done";
    default:
        return "unknown";
    }
}