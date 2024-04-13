void red_channel_client_pipe_add(RedChannelClient *rcc, PipeItem *item)
{
    spice_assert(rcc && item);
    rcc->pipe_size++;
    ring_add(&rcc->pipe, &item->link);
}
