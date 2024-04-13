void red_channel_push(RedChannel *channel)
{
    RingItem *link;
    RingItem *next;
    RedChannelClient *rcc;

    if (!channel) {
        return;
    }
    RING_FOREACH_SAFE(link, next, &channel->clients) {
        rcc = SPICE_CONTAINEROF(link, RedChannelClient, channel_link);
        red_channel_client_push(rcc);
    }
}
