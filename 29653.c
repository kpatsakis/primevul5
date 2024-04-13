void red_channel_apply_clients_data(RedChannel *channel, channel_client_callback_data cb, void *data)
{
    RingItem *link;
    RingItem *next;
    RedChannelClient *rcc;

    RING_FOREACH_SAFE(link, next, &channel->clients) {
        rcc = SPICE_CONTAINEROF(link, RedChannelClient, channel_link);
        cb(rcc, data);
    }
}
