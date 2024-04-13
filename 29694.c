int red_channel_client_pipe_item_is_linked(RedChannelClient *rcc,
                                           PipeItem *item)
{
    return ring_item_is_linked(&item->link);
}
