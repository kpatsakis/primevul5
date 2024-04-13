int red_channel_all_blocked(RedChannel *channel)
{
    RingItem *link;
    RedChannelClient *rcc;

    if (!channel || channel->clients_num == 0) {
        return FALSE;
    }
    RING_FOREACH(link, &channel->clients) {
        rcc = SPICE_CONTAINEROF(link, RedChannelClient, channel_link);
        if (!rcc->send_data.blocked) {
            return FALSE;
        }
    }
    return TRUE;
}
