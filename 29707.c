static void red_channel_client_send_empty_msg(RedChannelClient *rcc, PipeItem *base)
{
    EmptyMsgPipeItem *msg_pipe_item = SPICE_CONTAINEROF(base, EmptyMsgPipeItem, base);

    red_channel_client_init_send_data(rcc, msg_pipe_item->msg, NULL);
    red_channel_client_begin_send_message(rcc);
}
