void red_channel_destroy(RedChannel *channel)
{
    RingItem *link;
    RingItem *next;

    if (!channel) {
        return;
    }
    RING_FOREACH_SAFE(link, next, &channel->clients) {
        red_channel_client_destroy(
            SPICE_CONTAINEROF(link, RedChannelClient, channel_link));
    }

    red_channel_unref(channel);
}
