void red_channel_client_pipe_add_tail_no_push(RedChannelClient *rcc,
                                              PipeItem *item)
{
    spice_assert(rcc);
    rcc->pipe_size++;
    ring_add_before(&item->link, &rcc->pipe);
}
