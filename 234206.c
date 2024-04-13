int ssl3_read_n(SSL *s, int n, int max, int extend, int clearold)
{
    /*
     * If extend == 0, obtain new n-byte packet; if extend == 1, increase
     * packet by another n bytes. The packet will be in the sub-array of
     * s->s3->rbuf.buf specified by s->packet and s->packet_length. (If
     * s->rlayer.read_ahead is set, 'max' bytes may be stored in rbuf [plus
     * s->packet_length bytes if extend == 1].)
     * if clearold == 1, move the packet to the start of the buffer; if
     * clearold == 0 then leave any old packets where they were
     */
    int i, len, left;
    size_t align = 0;
    unsigned char *pkt;
    SSL3_BUFFER *rb;

    if (n <= 0)
        return n;

    rb = &s->rlayer.rbuf;
    if (rb->buf == NULL)
        if (!ssl3_setup_read_buffer(s))
            return -1;

    left = rb->left;
#if defined(SSL3_ALIGN_PAYLOAD) && SSL3_ALIGN_PAYLOAD!=0
    align = (size_t)rb->buf + SSL3_RT_HEADER_LENGTH;
    align = SSL3_ALIGN_PAYLOAD - 1 - ((align - 1) % SSL3_ALIGN_PAYLOAD);
#endif

    if (!extend) {
        /* start with empty packet ... */
        if (left == 0)
            rb->offset = align;
        else if (align != 0 && left >= SSL3_RT_HEADER_LENGTH) {
            /*
             * check if next packet length is large enough to justify payload
             * alignment...
             */
            pkt = rb->buf + rb->offset;
            if (pkt[0] == SSL3_RT_APPLICATION_DATA
                && (pkt[3] << 8 | pkt[4]) >= 128) {
                /*
                 * Note that even if packet is corrupted and its length field
                 * is insane, we can only be led to wrong decision about
                 * whether memmove will occur or not. Header values has no
                 * effect on memmove arguments and therefore no buffer
                 * overrun can be triggered.
                 */
                memmove(rb->buf + align, pkt, left);
                rb->offset = align;
            }
        }
        s->rlayer.packet = rb->buf + rb->offset;
        s->rlayer.packet_length = 0;
        /* ... now we can act as if 'extend' was set */
    }

    /*
     * For DTLS/UDP reads should not span multiple packets because the read
     * operation returns the whole packet at once (as long as it fits into
     * the buffer).
     */
    if (SSL_IS_DTLS(s)) {
        if (left == 0 && extend)
            return 0;
        if (left > 0 && n > left)
            n = left;
    }

    /* if there is enough in the buffer from a previous read, take some */
    if (left >= n) {
        s->rlayer.packet_length += n;
        rb->left = left - n;
        rb->offset += n;
        return (n);
    }

    /* else we need to read more data */

    len = s->rlayer.packet_length;
    pkt = rb->buf + align;
    /*
     * Move any available bytes to front of buffer: 'len' bytes already
     * pointed to by 'packet', 'left' extra ones at the end
     */
    if (s->rlayer.packet != pkt && clearold == 1) { /* len > 0 */
        memmove(pkt, s->rlayer.packet, len + left);
        s->rlayer.packet = pkt;
        rb->offset = len + align;
    }

    if (n > (int)(rb->len - rb->offset)) { /* does not happen */
        SSLerr(SSL_F_SSL3_READ_N, ERR_R_INTERNAL_ERROR);
        return -1;
    }

    /* We always act like read_ahead is set for DTLS */
    if (!s->rlayer.read_ahead && !SSL_IS_DTLS(s))
        /* ignore max parameter */
        max = n;
    else {
        if (max < n)
            max = n;
        if (max > (int)(rb->len - rb->offset))
            max = rb->len - rb->offset;
    }

    while (left < n) {
        /*
         * Now we have len+left bytes at the front of s->s3->rbuf.buf and
         * need to read in more until we have len+n (up to len+max if
         * possible)
         */

        clear_sys_error();
        if (s->rbio != NULL) {
            s->rwstate = SSL_READING;
            i = BIO_read(s->rbio, pkt + len + left, max - left);
        } else {
            SSLerr(SSL_F_SSL3_READ_N, SSL_R_READ_BIO_NOT_SET);
            i = -1;
        }

        if (i <= 0) {
            rb->left = left;
            if (s->mode & SSL_MODE_RELEASE_BUFFERS && !SSL_IS_DTLS(s))
                if (len + left == 0)
                    ssl3_release_read_buffer(s);
            return (i);
        }
        left += i;
        /*
         * reads should *never* span multiple packets for DTLS because the
         * underlying transport protocol is message oriented as opposed to
         * byte oriented as in the TLS case.
         */
        if (SSL_IS_DTLS(s)) {
            if (n > left)
                n = left;       /* makes the while condition false */
        }
    }

    /* done reading, now the book-keeping */
    rb->offset += n;
    rb->left = left - n;
    s->rlayer.packet_length += n;
    s->rwstate = SSL_NOTHING;
    return (n);
}