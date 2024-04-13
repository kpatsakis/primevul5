static void red_channel_add_client(RedChannel *channel, RedChannelClient *rcc)
{
    spice_assert(rcc);
    ring_add(&channel->clients, &rcc->channel_link);
    channel->clients_num++;
}
