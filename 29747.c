int red_channel_waits_for_migrate_data(RedChannel *channel)
{
    RedChannelClient *rcc;

    if (!red_channel_is_connected(channel)) {
        return FALSE;
    }

    if (channel->clients_num > 1) {
        return FALSE;
    }
    spice_assert(channel->clients_num == 1);
    rcc = SPICE_CONTAINEROF(ring_get_head(&channel->clients), RedChannelClient, channel_link);
    return red_channel_client_waits_for_migrate_data(rcc);
}
