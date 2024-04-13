void red_channel_client_pipe_add_tail(RedChannelClient *rcc, PipeItem *item)
{
    spice_assert(rcc);
    rcc->pipe_size++;
    ring_add_before(&item->link, &rcc->pipe);
    red_channel_client_push(rcc);
}
