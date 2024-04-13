static void reds_handle_ssl_accept(int fd, int event, void *data)
{
    RedLinkInfo *link = (RedLinkInfo *)data;
    int return_code;

    if ((return_code = SSL_accept(link->stream->ssl)) != 1) {
        int ssl_error = SSL_get_error(link->stream->ssl, return_code);
        if (ssl_error != SSL_ERROR_WANT_READ && ssl_error != SSL_ERROR_WANT_WRITE) {
            spice_warning("SSL_accept failed, error=%d", ssl_error);
            reds_link_free(link);
        } else {
            if (ssl_error == SSL_ERROR_WANT_READ) {
                core->watch_update_mask(link->stream->watch, SPICE_WATCH_EVENT_READ);
            } else {
                core->watch_update_mask(link->stream->watch, SPICE_WATCH_EVENT_WRITE);
            }
        }
        return;
    }
    reds_stream_remove_watch(link->stream);
    reds_handle_new_link(link);
}
