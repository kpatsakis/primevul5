static RedLinkInfo *reds_init_client_connection(int socket)
{
    RedLinkInfo *link;
    RedsStream *stream;
    int delay_val = 1;
    int flags;

    if ((flags = fcntl(socket, F_GETFL)) == -1) {
        spice_warning("accept failed, %s", strerror(errno));
        goto error;
    }

    if (fcntl(socket, F_SETFL, flags | O_NONBLOCK) == -1) {
        spice_warning("accept failed, %s", strerror(errno));
        goto error;
    }

    if (setsockopt(socket, IPPROTO_TCP, TCP_NODELAY, &delay_val, sizeof(delay_val)) == -1) {
        if (errno != ENOTSUP) {
            spice_warning("setsockopt failed, %s", strerror(errno));
        }
    }

    link = spice_new0(RedLinkInfo, 1);
    stream = spice_new0(RedsStream, 1);
    stream->info = spice_new0(SpiceChannelEventInfo, 1);
    link->stream = stream;

    stream->socket = socket;
    /* gather info + send event */

    /* deprecated fields. Filling them for backward compatibility */
    stream->info->llen = sizeof(stream->info->laddr);
    stream->info->plen = sizeof(stream->info->paddr);
    getsockname(stream->socket, (struct sockaddr*)(&stream->info->laddr), &stream->info->llen);
    getpeername(stream->socket, (struct sockaddr*)(&stream->info->paddr), &stream->info->plen);

    stream->info->flags |= SPICE_CHANNEL_EVENT_FLAG_ADDR_EXT;
    stream->info->llen_ext = sizeof(stream->info->laddr_ext);
    stream->info->plen_ext = sizeof(stream->info->paddr_ext);
    getsockname(stream->socket, (struct sockaddr*)(&stream->info->laddr_ext),
                &stream->info->llen_ext);
    getpeername(stream->socket, (struct sockaddr*)(&stream->info->paddr_ext),
                &stream->info->plen_ext);

    reds_stream_push_channel_event(stream, SPICE_CHANNEL_EVENT_CONNECTED);

    openssl_init(link);

    return link;

error:
    return NULL;
}
