SSL_SESSION *SSL_get1_session(SSL *ssl)
/* variant of SSL_get_session: caller really gets something */
{
    SSL_SESSION *sess;
    /*
     * Need to lock this all up rather than just use CRYPTO_add so that
     * somebody doesn't free ssl->session between when we check it's non-null
     * and when we up the reference count.
     */
    CRYPTO_w_lock(CRYPTO_LOCK_SSL_SESSION);
    sess = ssl->session;
    if (sess)
        sess->references++;
    CRYPTO_w_unlock(CRYPTO_LOCK_SSL_SESSION);
    return (sess);
}