void red_channel_client_destroy(RedChannelClient *rcc)
{
    rcc->destroying = 1;
    red_channel_client_disconnect(rcc);
    red_client_remove_channel(rcc);
    red_channel_client_unref(rcc);
}
