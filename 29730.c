uint32_t red_channel_min_pipe_size(RedChannel *channel)
{
    RingItem *link;
    RedChannelClient *rcc;
    uint32_t pipe_size = ~0;

    RING_FOREACH(link, &channel->clients) {
        rcc = SPICE_CONTAINEROF(link, RedChannelClient, channel_link);
        pipe_size = pipe_size < rcc->pipe_size ? pipe_size : rcc->pipe_size;
    }
    return pipe_size == ~0 ? 0 : pipe_size;
}
