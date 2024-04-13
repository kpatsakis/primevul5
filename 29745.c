int red_channel_test_remote_common_cap(RedChannel *channel, uint32_t cap)
{
    RingItem *link;

    RING_FOREACH(link, &channel->clients) {
        RedChannelClient *rcc = SPICE_CONTAINEROF(link, RedChannelClient, channel_link);

        if (!red_channel_client_test_remote_common_cap(rcc, cap)) {
            return FALSE;
        }
    }
    return TRUE;
}
