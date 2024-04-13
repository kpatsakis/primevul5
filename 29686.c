void red_channel_client_pipe_add_after(RedChannelClient *rcc,
                                       PipeItem *item, PipeItem *pos)
{
    spice_assert(rcc);
    spice_assert(pos);
    spice_assert(item);

    rcc->pipe_size++;
    ring_add_after(&item->link, &pos->link);
}
