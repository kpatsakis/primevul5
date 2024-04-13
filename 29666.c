static void red_channel_client_destroy_remote_caps(RedChannelClient* rcc)
{
    rcc->remote_caps.num_common_caps = 0;
    free(rcc->remote_caps.common_caps);
    rcc->remote_caps.num_caps = 0;
    free(rcc->remote_caps.caps);
}
