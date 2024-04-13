SPICE_GNUC_VISIBLE int spice_server_add_client(SpiceServer *s, int socket, int skip_auth)
{
    RedLinkInfo *link;
    RedsStream *stream;

    spice_assert(reds == s);
    if (!(link = reds_init_client_connection(socket))) {
        spice_warning("accept failed");
        return -1;
    }

    link->skip_auth = skip_auth;

    stream = link->stream;
    stream->read = stream_read_cb;
    stream->write = stream_write_cb;
    stream->writev = stream_writev_cb;

    reds_handle_new_link(link);
    return 0;
}
