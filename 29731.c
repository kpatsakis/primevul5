int red_channel_no_item_being_sent(RedChannel *channel)
{
    RingItem *link;
    RedChannelClient *rcc;

    RING_FOREACH(link, &channel->clients) {
        rcc = SPICE_CONTAINEROF(link, RedChannelClient, channel_link);
        if (!red_channel_client_no_item_being_sent(rcc)) {
            return FALSE;
        }
    }
    return TRUE;
}
