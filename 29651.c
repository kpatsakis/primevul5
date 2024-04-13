int red_channel_any_blocked(RedChannel *channel)
{
    RingItem *link;
    RedChannelClient *rcc;

    RING_FOREACH(link, &channel->clients) {
        rcc = SPICE_CONTAINEROF(link, RedChannelClient, channel_link);
        if (rcc->send_data.blocked) {
            return TRUE;
        }
    }
    return FALSE;
}
