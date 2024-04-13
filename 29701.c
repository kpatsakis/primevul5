void red_channel_client_receive(RedChannelClient *rcc)
{
    red_channel_client_ref(rcc);
    red_peer_handle_incoming(rcc->stream, &rcc->incoming);
    red_channel_client_unref(rcc);
}
