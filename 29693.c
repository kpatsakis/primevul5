static inline PipeItem *red_channel_client_pipe_item_get(RedChannelClient *rcc)
{
    PipeItem *item;

    if (!rcc || rcc->send_data.blocked
             || red_channel_client_waiting_for_ack(rcc)
             || !(item = (PipeItem *)ring_get_tail(&rcc->pipe))) {
        return NULL;
    }
    red_channel_client_pipe_remove(rcc, item);
    return item;
}
