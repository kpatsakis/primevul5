int ssl3_read_bytes(SSL *s, int type, int *recvd_type, unsigned char *buf,
                    int len, int peek)
{
    int al, i, j, ret;
    unsigned int n, curr_rec, num_recs, read_bytes;
    SSL3_RECORD *rr;
    SSL3_BUFFER *rbuf;
    void (*cb) (const SSL *ssl, int type2, int val) = NULL;

    rbuf = &s->rlayer.rbuf;

    if (!SSL3_BUFFER_is_initialised(rbuf)) {
        /* Not initialized yet */
        if (!ssl3_setup_read_buffer(s))
            return (-1);
    }

    if ((type && (type != SSL3_RT_APPLICATION_DATA)
         && (type != SSL3_RT_HANDSHAKE)) || (peek
                                             && (type !=
                                                 SSL3_RT_APPLICATION_DATA))) {
        SSLerr(SSL_F_SSL3_READ_BYTES, ERR_R_INTERNAL_ERROR);
        return -1;
    }

    if ((type == SSL3_RT_HANDSHAKE) && (s->rlayer.handshake_fragment_len > 0))
        /* (partially) satisfy request from storage */
    {
        unsigned char *src = s->rlayer.handshake_fragment;
        unsigned char *dst = buf;
        unsigned int k;

        /* peek == 0 */
        n = 0;
        while ((len > 0) && (s->rlayer.handshake_fragment_len > 0)) {
            *dst++ = *src++;
            len--;
            s->rlayer.handshake_fragment_len--;
            n++;
        }
        /* move any remaining fragment bytes: */
        for (k = 0; k < s->rlayer.handshake_fragment_len; k++)
            s->rlayer.handshake_fragment[k] = *src++;

        if (recvd_type != NULL)
            *recvd_type = SSL3_RT_HANDSHAKE;

        return n;
    }

    /*
     * Now s->rlayer.handshake_fragment_len == 0 if type == SSL3_RT_HANDSHAKE.
     */

    if (!ossl_statem_get_in_handshake(s) && SSL_in_init(s)) {
        /* type == SSL3_RT_APPLICATION_DATA */
        i = s->handshake_func(s);
        if (i < 0)
            return (i);
        if (i == 0) {
            SSLerr(SSL_F_SSL3_READ_BYTES, SSL_R_SSL_HANDSHAKE_FAILURE);
            return (-1);
        }
    }
 start:
    s->rwstate = SSL_NOTHING;

    /*-
     * For each record 'i' up to |num_recs]
     * rr[i].type     - is the type of record
     * rr[i].data,    - data
     * rr[i].off,     - offset into 'data' for next read
     * rr[i].length,  - number of bytes.
     */
    rr = s->rlayer.rrec;
    num_recs = RECORD_LAYER_get_numrpipes(&s->rlayer);

    do {
        /* get new records if necessary */
        if (num_recs == 0) {
            ret = ssl3_get_record(s);
            if (ret <= 0)
                return (ret);
            num_recs = RECORD_LAYER_get_numrpipes(&s->rlayer);
            if (num_recs == 0) {
                /* Shouldn't happen */
                al = SSL_AD_INTERNAL_ERROR;
                SSLerr(SSL_F_SSL3_READ_BYTES, ERR_R_INTERNAL_ERROR);
                goto f_err;
            }
        }
        /* Skip over any records we have already read */
        for (curr_rec = 0;
             curr_rec < num_recs && SSL3_RECORD_is_read(&rr[curr_rec]);
             curr_rec++) ;
        if (curr_rec == num_recs) {
            RECORD_LAYER_set_numrpipes(&s->rlayer, 0);
            num_recs = 0;
            curr_rec = 0;
        }
    } while (num_recs == 0);
    rr = &rr[curr_rec];

    /*
     * Reset the count of consecutive warning alerts if we've got a non-empty
     * record that isn't an alert.
     */
    if (SSL3_RECORD_get_type(rr) != SSL3_RT_ALERT
            && SSL3_RECORD_get_length(rr) != 0)
        s->rlayer.alert_count = 0;

    /* we now have a packet which can be read and processed */

    if (s->s3->change_cipher_spec /* set when we receive ChangeCipherSpec,
                                   * reset by ssl3_get_finished */
        && (SSL3_RECORD_get_type(rr) != SSL3_RT_HANDSHAKE)) {
        al = SSL_AD_UNEXPECTED_MESSAGE;
        SSLerr(SSL_F_SSL3_READ_BYTES, SSL_R_DATA_BETWEEN_CCS_AND_FINISHED);
        goto f_err;
    }

    /*
     * If the other end has shut down, throw anything we read away (even in
     * 'peek' mode)
     */
    if (s->shutdown & SSL_RECEIVED_SHUTDOWN) {
        SSL3_RECORD_set_length(rr, 0);
        s->rwstate = SSL_NOTHING;
        return (0);
    }

    if (type == SSL3_RECORD_get_type(rr)
        || (SSL3_RECORD_get_type(rr) == SSL3_RT_CHANGE_CIPHER_SPEC
            && type == SSL3_RT_HANDSHAKE && recvd_type != NULL)) {
        /*
         * SSL3_RT_APPLICATION_DATA or
         * SSL3_RT_HANDSHAKE or
         * SSL3_RT_CHANGE_CIPHER_SPEC
         */
        /*
         * make sure that we are not getting application data when we are
         * doing a handshake for the first time
         */
        if (SSL_in_init(s) && (type == SSL3_RT_APPLICATION_DATA) &&
            (s->enc_read_ctx == NULL)) {
            al = SSL_AD_UNEXPECTED_MESSAGE;
            SSLerr(SSL_F_SSL3_READ_BYTES, SSL_R_APP_DATA_IN_HANDSHAKE);
            goto f_err;
        }

        if (type == SSL3_RT_HANDSHAKE
            && SSL3_RECORD_get_type(rr) == SSL3_RT_CHANGE_CIPHER_SPEC
            && s->rlayer.handshake_fragment_len > 0) {
            al = SSL_AD_UNEXPECTED_MESSAGE;
            SSLerr(SSL_F_SSL3_READ_BYTES, SSL_R_CCS_RECEIVED_EARLY);
            goto f_err;
        }

        if (recvd_type != NULL)
            *recvd_type = SSL3_RECORD_get_type(rr);

        if (len <= 0)
            return (len);

        read_bytes = 0;
        do {
            if ((unsigned int)len - read_bytes > SSL3_RECORD_get_length(rr))
                n = SSL3_RECORD_get_length(rr);
            else
                n = (unsigned int)len - read_bytes;

            memcpy(buf, &(rr->data[rr->off]), n);
            buf += n;
            if (peek) {
                /* Mark any zero length record as consumed CVE-2016-6305 */
                if (SSL3_RECORD_get_length(rr) == 0)
                    SSL3_RECORD_set_read(rr);
            } else {
                SSL3_RECORD_sub_length(rr, n);
                SSL3_RECORD_add_off(rr, n);
                if (SSL3_RECORD_get_length(rr) == 0) {
                    s->rlayer.rstate = SSL_ST_READ_HEADER;
                    SSL3_RECORD_set_off(rr, 0);
                    SSL3_RECORD_set_read(rr);
                }
            }
            if (SSL3_RECORD_get_length(rr) == 0
                || (peek && n == SSL3_RECORD_get_length(rr))) {
                curr_rec++;
                rr++;
            }
            read_bytes += n;
        } while (type == SSL3_RT_APPLICATION_DATA && curr_rec < num_recs
                 && read_bytes < (unsigned int)len);
        if (read_bytes == 0) {
            /* We must have read empty records. Get more data */
            goto start;
        }
        if (!peek && curr_rec == num_recs
            && (s->mode & SSL_MODE_RELEASE_BUFFERS)
            && SSL3_BUFFER_get_left(rbuf) == 0)
            ssl3_release_read_buffer(s);
        return read_bytes;
    }

    /*
     * If we get here, then type != rr->type; if we have a handshake message,
     * then it was unexpected (Hello Request or Client Hello) or invalid (we
     * were actually expecting a CCS).
     */

    /*
     * Lets just double check that we've not got an SSLv2 record
     */
    if (rr->rec_version == SSL2_VERSION) {
        /*
         * Should never happen. ssl3_get_record() should only give us an SSLv2
         * record back if this is the first packet and we are looking for an
         * initial ClientHello. Therefore |type| should always be equal to
         * |rr->type|. If not then something has gone horribly wrong
         */
        al = SSL_AD_INTERNAL_ERROR;
        SSLerr(SSL_F_SSL3_READ_BYTES, ERR_R_INTERNAL_ERROR);
        goto f_err;
    }

    if (s->method->version == TLS_ANY_VERSION
        && (s->server || rr->type != SSL3_RT_ALERT)) {
        /*
         * If we've got this far and still haven't decided on what version
         * we're using then this must be a client side alert we're dealing with
         * (we don't allow heartbeats yet). We shouldn't be receiving anything
         * other than a ClientHello if we are a server.
         */
        s->version = rr->rec_version;
        al = SSL_AD_UNEXPECTED_MESSAGE;
        SSLerr(SSL_F_SSL3_READ_BYTES, SSL_R_UNEXPECTED_MESSAGE);
        goto f_err;
    }

    /*
     * In case of record types for which we have 'fragment' storage, fill
     * that so that we can process the data at a fixed place.
     */
    {
        unsigned int dest_maxlen = 0;
        unsigned char *dest = NULL;
        unsigned int *dest_len = NULL;

        if (SSL3_RECORD_get_type(rr) == SSL3_RT_HANDSHAKE) {
            dest_maxlen = sizeof s->rlayer.handshake_fragment;
            dest = s->rlayer.handshake_fragment;
            dest_len = &s->rlayer.handshake_fragment_len;
        } else if (SSL3_RECORD_get_type(rr) == SSL3_RT_ALERT) {
            dest_maxlen = sizeof s->rlayer.alert_fragment;
            dest = s->rlayer.alert_fragment;
            dest_len = &s->rlayer.alert_fragment_len;
        }

        if (dest_maxlen > 0) {
            n = dest_maxlen - *dest_len; /* available space in 'dest' */
            if (SSL3_RECORD_get_length(rr) < n)
                n = SSL3_RECORD_get_length(rr); /* available bytes */

            /* now move 'n' bytes: */
            while (n-- > 0) {
                dest[(*dest_len)++] =
                    SSL3_RECORD_get_data(rr)[SSL3_RECORD_get_off(rr)];
                SSL3_RECORD_add_off(rr, 1);
                SSL3_RECORD_add_length(rr, -1);
            }

            if (*dest_len < dest_maxlen) {
                SSL3_RECORD_set_read(rr);
                goto start;     /* fragment was too small */
            }
        }
    }

    /*-
     * s->rlayer.handshake_fragment_len == 4  iff  rr->type == SSL3_RT_HANDSHAKE;
     * s->rlayer.alert_fragment_len == 2      iff  rr->type == SSL3_RT_ALERT.
     * (Possibly rr is 'empty' now, i.e. rr->length may be 0.)
     */

    /* If we are a client, check for an incoming 'Hello Request': */
    if ((!s->server) &&
        (s->rlayer.handshake_fragment_len >= 4) &&
        (s->rlayer.handshake_fragment[0] == SSL3_MT_HELLO_REQUEST) &&
        (s->session != NULL) && (s->session->cipher != NULL)) {
        s->rlayer.handshake_fragment_len = 0;

        if ((s->rlayer.handshake_fragment[1] != 0) ||
            (s->rlayer.handshake_fragment[2] != 0) ||
            (s->rlayer.handshake_fragment[3] != 0)) {
            al = SSL_AD_DECODE_ERROR;
            SSLerr(SSL_F_SSL3_READ_BYTES, SSL_R_BAD_HELLO_REQUEST);
            goto f_err;
        }

        if (s->msg_callback)
            s->msg_callback(0, s->version, SSL3_RT_HANDSHAKE,
                            s->rlayer.handshake_fragment, 4, s,
                            s->msg_callback_arg);

        if (SSL_is_init_finished(s) &&
            !(s->s3->flags & SSL3_FLAGS_NO_RENEGOTIATE_CIPHERS) &&
            !s->s3->renegotiate) {
            ssl3_renegotiate(s);
            if (ssl3_renegotiate_check(s)) {
                i = s->handshake_func(s);
                if (i < 0)
                    return (i);
                if (i == 0) {
                    SSLerr(SSL_F_SSL3_READ_BYTES, SSL_R_SSL_HANDSHAKE_FAILURE);
                    return (-1);
                }

                if (!(s->mode & SSL_MODE_AUTO_RETRY)) {
                    if (SSL3_BUFFER_get_left(rbuf) == 0) {
                        /* no read-ahead left? */
                        BIO *bio;
                        /*
                         * In the case where we try to read application data,
                         * but we trigger an SSL handshake, we return -1 with
                         * the retry option set.  Otherwise renegotiation may
                         * cause nasty problems in the blocking world
                         */
                        s->rwstate = SSL_READING;
                        bio = SSL_get_rbio(s);
                        BIO_clear_retry_flags(bio);
                        BIO_set_retry_read(bio);
                        return (-1);
                    }
                }
            }
        }
        /*
         * we either finished a handshake or ignored the request, now try
         * again to obtain the (application) data we were asked for
         */
        goto start;
    }
    /*
     * If we are a server and get a client hello when renegotiation isn't
     * allowed send back a no renegotiation alert and carry on. WARNING:
     * experimental code, needs reviewing (steve)
     */
    if (s->server &&
        SSL_is_init_finished(s) &&
        !s->s3->send_connection_binding &&
        (s->version > SSL3_VERSION) &&
        (s->rlayer.handshake_fragment_len >= 4) &&
        (s->rlayer.handshake_fragment[0] == SSL3_MT_CLIENT_HELLO) &&
        (s->session != NULL) && (s->session->cipher != NULL) &&
        !(s->ctx->options & SSL_OP_ALLOW_UNSAFE_LEGACY_RENEGOTIATION)) {
        SSL3_RECORD_set_length(rr, 0);
        SSL3_RECORD_set_read(rr);
        ssl3_send_alert(s, SSL3_AL_WARNING, SSL_AD_NO_RENEGOTIATION);
        goto start;
    }
    if (s->rlayer.alert_fragment_len >= 2) {
        int alert_level = s->rlayer.alert_fragment[0];
        int alert_descr = s->rlayer.alert_fragment[1];

        s->rlayer.alert_fragment_len = 0;

        if (s->msg_callback)
            s->msg_callback(0, s->version, SSL3_RT_ALERT,
                            s->rlayer.alert_fragment, 2, s,
                            s->msg_callback_arg);

        if (s->info_callback != NULL)
            cb = s->info_callback;
        else if (s->ctx->info_callback != NULL)
            cb = s->ctx->info_callback;

        if (cb != NULL) {
            j = (alert_level << 8) | alert_descr;
            cb(s, SSL_CB_READ_ALERT, j);
        }

        if (alert_level == SSL3_AL_WARNING) {
            s->s3->warn_alert = alert_descr;
            SSL3_RECORD_set_read(rr);

            s->rlayer.alert_count++;
            if (s->rlayer.alert_count == MAX_WARN_ALERT_COUNT) {
                al = SSL_AD_UNEXPECTED_MESSAGE;
                SSLerr(SSL_F_SSL3_READ_BYTES, SSL_R_TOO_MANY_WARN_ALERTS);
                goto f_err;
            }

            if (alert_descr == SSL_AD_CLOSE_NOTIFY) {
                s->shutdown |= SSL_RECEIVED_SHUTDOWN;
                return (0);
            }
            /*
             * This is a warning but we receive it if we requested
             * renegotiation and the peer denied it. Terminate with a fatal
             * alert because if application tried to renegotiate it
             * presumably had a good reason and expects it to succeed. In
             * future we might have a renegotiation where we don't care if
             * the peer refused it where we carry on.
             */
            else if (alert_descr == SSL_AD_NO_RENEGOTIATION) {
                al = SSL_AD_HANDSHAKE_FAILURE;
                SSLerr(SSL_F_SSL3_READ_BYTES, SSL_R_NO_RENEGOTIATION);
                goto f_err;
            }
#ifdef SSL_AD_MISSING_SRP_USERNAME
            else if (alert_descr == SSL_AD_MISSING_SRP_USERNAME)
                return (0);
#endif
        } else if (alert_level == SSL3_AL_FATAL) {
            char tmp[16];

            s->rwstate = SSL_NOTHING;
            s->s3->fatal_alert = alert_descr;
            SSLerr(SSL_F_SSL3_READ_BYTES, SSL_AD_REASON_OFFSET + alert_descr);
            BIO_snprintf(tmp, sizeof tmp, "%d", alert_descr);
            ERR_add_error_data(2, "SSL alert number ", tmp);
            s->shutdown |= SSL_RECEIVED_SHUTDOWN;
            SSL3_RECORD_set_read(rr);
            SSL_CTX_remove_session(s->session_ctx, s->session);
            return (0);
        } else {
            al = SSL_AD_ILLEGAL_PARAMETER;
            SSLerr(SSL_F_SSL3_READ_BYTES, SSL_R_UNKNOWN_ALERT_TYPE);
            goto f_err;
        }

        goto start;
    }

    if (s->shutdown & SSL_SENT_SHUTDOWN) { /* but we have not received a
                                            * shutdown */
        s->rwstate = SSL_NOTHING;
        SSL3_RECORD_set_length(rr, 0);
        SSL3_RECORD_set_read(rr);
        return (0);
    }

    if (SSL3_RECORD_get_type(rr) == SSL3_RT_CHANGE_CIPHER_SPEC) {
        al = SSL_AD_UNEXPECTED_MESSAGE;
        SSLerr(SSL_F_SSL3_READ_BYTES, SSL_R_CCS_RECEIVED_EARLY);
        goto f_err;
    }

    /*
     * Unexpected handshake message (Client Hello, or protocol violation)
     */
    if ((s->rlayer.handshake_fragment_len >= 4)
        && !ossl_statem_get_in_handshake(s)) {
        if (SSL_is_init_finished(s) &&
            !(s->s3->flags & SSL3_FLAGS_NO_RENEGOTIATE_CIPHERS)) {
            ossl_statem_set_in_init(s, 1);
            s->renegotiate = 1;
            s->new_session = 1;
        }
        i = s->handshake_func(s);
        if (i < 0)
            return (i);
        if (i == 0) {
            SSLerr(SSL_F_SSL3_READ_BYTES, SSL_R_SSL_HANDSHAKE_FAILURE);
            return (-1);
        }

        if (!(s->mode & SSL_MODE_AUTO_RETRY)) {
            if (SSL3_BUFFER_get_left(rbuf) == 0) {
                /* no read-ahead left? */
                BIO *bio;
                /*
                 * In the case where we try to read application data, but we
                 * trigger an SSL handshake, we return -1 with the retry
                 * option set.  Otherwise renegotiation may cause nasty
                 * problems in the blocking world
                 */
                s->rwstate = SSL_READING;
                bio = SSL_get_rbio(s);
                BIO_clear_retry_flags(bio);
                BIO_set_retry_read(bio);
                return (-1);
            }
        }
        goto start;
    }

    switch (SSL3_RECORD_get_type(rr)) {
    default:
        /*
         * TLS up to v1.1 just ignores unknown message types: TLS v1.2 give
         * an unexpected message alert.
         */
        if (s->version >= TLS1_VERSION && s->version <= TLS1_1_VERSION) {
            SSL3_RECORD_set_length(rr, 0);
            SSL3_RECORD_set_read(rr);
            goto start;
        }
        al = SSL_AD_UNEXPECTED_MESSAGE;
        SSLerr(SSL_F_SSL3_READ_BYTES, SSL_R_UNEXPECTED_RECORD);
        goto f_err;
    case SSL3_RT_CHANGE_CIPHER_SPEC:
    case SSL3_RT_ALERT:
    case SSL3_RT_HANDSHAKE:
        /*
         * we already handled all of these, with the possible exception of
         * SSL3_RT_HANDSHAKE when ossl_statem_get_in_handshake(s) is true, but
         * that should not happen when type != rr->type
         */
        al = SSL_AD_UNEXPECTED_MESSAGE;
        SSLerr(SSL_F_SSL3_READ_BYTES, ERR_R_INTERNAL_ERROR);
        goto f_err;
    case SSL3_RT_APPLICATION_DATA:
        /*
         * At this point, we were expecting handshake data, but have
         * application data.  If the library was running inside ssl3_read()
         * (i.e. in_read_app_data is set) and it makes sense to read
         * application data at this point (session renegotiation not yet
         * started), we will indulge it.
         */
        if (ossl_statem_app_data_allowed(s)) {
            s->s3->in_read_app_data = 2;
            return (-1);
        } else {
            al = SSL_AD_UNEXPECTED_MESSAGE;
            SSLerr(SSL_F_SSL3_READ_BYTES, SSL_R_UNEXPECTED_RECORD);
            goto f_err;
        }
    }
    /* not reached */

 f_err:
    ssl3_send_alert(s, SSL3_AL_FATAL, al);
    return (-1);
}