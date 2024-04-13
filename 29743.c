uint32_t red_channel_sum_pipes_size(RedChannel *channel)
{
    RingItem *link;
    RedChannelClient *rcc;
    uint32_t sum = 0;

    RING_FOREACH(link, &channel->clients) {
        rcc = SPICE_CONTAINEROF(link, RedChannelClient, channel_link);
        sum += rcc->pipe_size;
    }
    return sum;
}
