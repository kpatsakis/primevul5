void SSL_CTX_sess_set_new_cb(SSL_CTX *ctx,
                             int (*cb) (struct ssl_st *ssl,
                                        SSL_SESSION *sess))
{
    ctx->new_session_cb = cb;
}