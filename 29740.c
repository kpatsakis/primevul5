void red_channel_send(RedChannel *channel)
{
    RingItem *link;
    RingItem *next;

    RING_FOREACH_SAFE(link, next, &channel->clients) {
        red_channel_client_send(SPICE_CONTAINEROF(link, RedChannelClient, channel_link));
    }
}
