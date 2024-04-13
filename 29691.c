void red_channel_client_pipe_add_type(RedChannelClient *rcc, int pipe_item_type)
{
    PipeItem *item = spice_new(PipeItem, 1);

    red_channel_pipe_item_init(rcc->channel, item, pipe_item_type);
    red_channel_client_pipe_add(rcc, item);
    red_channel_client_push(rcc);
}
