int SSL_set_session_secret_cb(SSL *s,
                              int (*tls_session_secret_cb) (SSL *s,
                                                            void *secret,
                                                            int *secret_len,
                                                            STACK_OF(SSL_CIPHER)
                                                            *peer_ciphers,
                                                            SSL_CIPHER
                                                            **cipher,
                                                            void *arg),
                              void *arg)
{
    if (s == NULL)
        return (0);
    s->tls_session_secret_cb = tls_session_secret_cb;
    s->tls_session_secret_cb_arg = arg;
    return (1);
}