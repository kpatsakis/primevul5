static void red_channel_client_default_peer_on_error(RedChannelClient *rcc)
{
    red_channel_client_disconnect(rcc);
}
