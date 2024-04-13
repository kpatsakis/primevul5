static RedLinkInfo *reds_init_client_ssl_connection(int socket)
{
    RedLinkInfo *link;
    int return_code;
    int ssl_error;
    BIO *sbio;

    link = reds_init_client_connection(socket);
    if (link == NULL)
        goto error;

    if (!(sbio = BIO_new_socket(link->stream->socket, BIO_NOCLOSE))) {
        spice_warning("could not allocate ssl bio socket");
        goto error;
    }

    link->stream->ssl = SSL_new(reds->ctx);
    if (!link->stream->ssl) {
        spice_warning("could not allocate ssl context");
        BIO_free(sbio);
        goto error;
    }

    SSL_set_bio(link->stream->ssl, sbio, sbio);

    link->stream->write = stream_ssl_write_cb;
    link->stream->read = stream_ssl_read_cb;
    link->stream->writev = NULL;

    return_code = SSL_accept(link->stream->ssl);
    if (return_code == 1) {
        reds_handle_new_link(link);
        return link;
    }

    ssl_error = SSL_get_error(link->stream->ssl, return_code);
    if (return_code == -1 && (ssl_error == SSL_ERROR_WANT_READ ||
                              ssl_error == SSL_ERROR_WANT_WRITE)) {
        int eventmask = ssl_error == SSL_ERROR_WANT_READ ?
            SPICE_WATCH_EVENT_READ : SPICE_WATCH_EVENT_WRITE;
        link->stream->watch = core->watch_add(link->stream->socket, eventmask,
                                            reds_handle_ssl_accept, link);
        return link;
    }

    ERR_print_errors_fp(stderr);
    spice_warning("SSL_accept failed, error=%d", ssl_error);
    SSL_free(link->stream->ssl);

error:
    free(link->stream);
    BN_free(link->tiTicketing.bn);
    free(link);
    return NULL;
}
