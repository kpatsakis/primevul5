void red_channel_client_pipe_clear(RedChannelClient *rcc)
{
    PipeItem *item;

    if (rcc) {
        red_channel_client_clear_sent_item(rcc);
    }
    while ((item = (PipeItem *)ring_get_head(&rcc->pipe))) {
        ring_remove(&item->link);
        red_channel_client_release_item(rcc, item, FALSE);
    }
    rcc->pipe_size = 0;
}
