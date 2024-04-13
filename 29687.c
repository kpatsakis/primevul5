void red_channel_client_pipe_add_empty_msg(RedChannelClient *rcc, int msg_type)
{
    EmptyMsgPipeItem *item = spice_new(EmptyMsgPipeItem, 1);

    red_channel_pipe_item_init(rcc->channel, &item->base, PIPE_ITEM_TYPE_EMPTY_MSG);
    item->msg = msg_type;
    red_channel_client_pipe_add(rcc, &item->base);
    red_channel_client_push(rcc);
}
