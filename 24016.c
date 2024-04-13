static int remove_session_lock(SSL_CTX *ctx, SSL_SESSION *c, int lck)
{
    SSL_SESSION *r;
    int ret = 0;

    if ((c != NULL) && (c->session_id_length != 0)) {
        if (lck)
            CRYPTO_w_lock(CRYPTO_LOCK_SSL_CTX);
        if ((r = lh_SSL_SESSION_retrieve(ctx->sessions, c)) == c) {
            ret = 1;
            r = lh_SSL_SESSION_delete(ctx->sessions, c);
            SSL_SESSION_list_remove(ctx, c);
        }

        if (lck)
            CRYPTO_w_unlock(CRYPTO_LOCK_SSL_CTX);

        if (ret) {
            r->not_resumable = 1;
            if (ctx->remove_session_cb != NULL)
                ctx->remove_session_cb(ctx, r);
            SSL_SESSION_free(r);
        }
    } else
        ret = 0;
    return (ret);
}