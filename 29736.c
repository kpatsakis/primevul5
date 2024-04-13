void red_channel_receive(RedChannel *channel)
{
    RingItem *link;
    RingItem *next;
    RedChannelClient *rcc;

    RING_FOREACH_SAFE(link, next, &channel->clients) {
        rcc = SPICE_CONTAINEROF(link, RedChannelClient, channel_link);
        red_channel_client_receive(rcc);
    }
}
