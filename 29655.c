void red_channel_client_ack_zero_messages_window(RedChannelClient *rcc)
{
    rcc->ack_data.messages_window = 0;
}
