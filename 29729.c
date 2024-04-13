void red_channel_init_outgoing_messages_window(RedChannel *channel)
{
    RingItem *link;
    RingItem *next;

    RING_FOREACH_SAFE(link, next, &channel->clients) {
        red_channel_client_init_outgoing_messages_window(
            SPICE_CONTAINEROF(link, RedChannelClient, channel_link));
    }
}
