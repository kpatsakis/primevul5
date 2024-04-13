int ssl3_write_pending(SSL *s, int type, const unsigned char *buf,
                       unsigned int len)
{
    int i;
    SSL3_BUFFER *wb = s->rlayer.wbuf;
    unsigned int currbuf = 0;

/* XXXX */
    if ((s->rlayer.wpend_tot > (int)len)
        || ((s->rlayer.wpend_buf != buf) &&
            !(s->mode & SSL_MODE_ACCEPT_MOVING_WRITE_BUFFER))
        || (s->rlayer.wpend_type != type)) {
        SSLerr(SSL_F_SSL3_WRITE_PENDING, SSL_R_BAD_WRITE_RETRY);
        return (-1);
    }

    for (;;) {
        /* Loop until we find a buffer we haven't written out yet */
        if (SSL3_BUFFER_get_left(&wb[currbuf]) == 0
            && currbuf < s->rlayer.numwpipes - 1) {
            currbuf++;
            continue;
        }
        clear_sys_error();
        if (s->wbio != NULL) {
            s->rwstate = SSL_WRITING;
            i = BIO_write(s->wbio, (char *)
                          &(SSL3_BUFFER_get_buf(&wb[currbuf])
                            [SSL3_BUFFER_get_offset(&wb[currbuf])]),
                          (unsigned int)SSL3_BUFFER_get_left(&wb[currbuf]));
        } else {
            SSLerr(SSL_F_SSL3_WRITE_PENDING, SSL_R_BIO_NOT_SET);
            i = -1;
        }
        if (i == SSL3_BUFFER_get_left(&wb[currbuf])) {
            SSL3_BUFFER_set_left(&wb[currbuf], 0);
            SSL3_BUFFER_add_offset(&wb[currbuf], i);
            if (currbuf + 1 < s->rlayer.numwpipes)
                continue;
            s->rwstate = SSL_NOTHING;
            return (s->rlayer.wpend_ret);
        } else if (i <= 0) {
            if (SSL_IS_DTLS(s)) {
                /*
                 * For DTLS, just drop it. That's kind of the whole point in
                 * using a datagram service
                 */
                SSL3_BUFFER_set_left(&wb[currbuf], 0);
            }
            return (i);
        }
        SSL3_BUFFER_add_offset(&wb[currbuf], i);
        SSL3_BUFFER_add_left(&wb[currbuf], -i);
    }
}