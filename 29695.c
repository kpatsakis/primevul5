static void red_channel_client_pipe_remove(RedChannelClient *rcc, PipeItem *item)
{
    rcc->pipe_size--;
    ring_remove(&item->link);
}
