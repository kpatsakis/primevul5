int SSL_set_session_ticket_ext_cb(SSL *s, tls_session_ticket_ext_cb_fn cb,
                                  void *arg)
{
    if (s == NULL)
        return (0);
    s->tls_session_ticket_ext_cb = cb;
    s->tls_session_ticket_ext_cb_arg = arg;
    return (1);
}