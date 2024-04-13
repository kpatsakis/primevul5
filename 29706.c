void red_channel_client_send(RedChannelClient *rcc)
{
    red_channel_client_ref(rcc);
    red_peer_handle_outgoing(rcc->stream, &rcc->outgoing);
    red_channel_client_unref(rcc);
}
