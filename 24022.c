X509 *SSL_SESSION_get0_peer(SSL_SESSION *s)
{
    return s->peer;
}