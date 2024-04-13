int red_channel_client_send_message_pending(RedChannelClient *rcc)
{
    return rcc->send_data.header.get_msg_type(&rcc->send_data.header) != 0;
}
