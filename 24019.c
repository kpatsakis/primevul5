int SSL_set_session(SSL *s, SSL_SESSION *session)
{
    int ret = 0;
    const SSL_METHOD *meth;

    if (session != NULL) {
        meth = s->ctx->method->get_ssl_method(session->ssl_version);
        if (meth == NULL)
            meth = s->method->get_ssl_method(session->ssl_version);
        if (meth == NULL) {
            SSLerr(SSL_F_SSL_SET_SESSION, SSL_R_UNABLE_TO_FIND_SSL_METHOD);
            return (0);
        }

        if (meth != s->method) {
            if (!SSL_set_ssl_method(s, meth))
                return (0);
        }
#ifndef OPENSSL_NO_KRB5
        if (s->kssl_ctx && !s->kssl_ctx->client_princ &&
            session->krb5_client_princ_len > 0) {
            s->kssl_ctx->client_princ =
                (char *)OPENSSL_malloc(session->krb5_client_princ_len + 1);
            memcpy(s->kssl_ctx->client_princ, session->krb5_client_princ,
                   session->krb5_client_princ_len);
            s->kssl_ctx->client_princ[session->krb5_client_princ_len] = '\0';
        }
#endif                          /* OPENSSL_NO_KRB5 */

        /* CRYPTO_w_lock(CRYPTO_LOCK_SSL); */
        CRYPTO_add(&session->references, 1, CRYPTO_LOCK_SSL_SESSION);
        if (s->session != NULL)
            SSL_SESSION_free(s->session);
        s->session = session;
        s->verify_result = s->session->verify_result;
        /* CRYPTO_w_unlock(CRYPTO_LOCK_SSL); */
        ret = 1;
    } else {
        if (s->session != NULL) {
            SSL_SESSION_free(s->session);
            s->session = NULL;
        }

        meth = s->ctx->method;
        if (meth != s->method) {
            if (!SSL_set_ssl_method(s, meth))
                return (0);
        }
        ret = 1;
    }
    return (ret);
}