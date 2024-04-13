static void reds_info_new_channel(RedLinkInfo *link, int connection_id)
{
    spice_info("channel %d:%d, connected successfully, over %s link",
               link->link_mess->channel_type,
               link->link_mess->channel_id,
               link->stream->ssl == NULL ? "Non Secure" : "Secure");
    /* add info + send event */
    if (link->stream->ssl) {
        link->stream->info->flags |= SPICE_CHANNEL_EVENT_FLAG_TLS;
    }
    link->stream->info->connection_id = connection_id;
    link->stream->info->type = link->link_mess->channel_type;
    link->stream->info->id   = link->link_mess->channel_id;
    reds_stream_push_channel_event(link->stream, SPICE_CHANNEL_EVENT_INITIALIZED);
}
