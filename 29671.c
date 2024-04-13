uint64_t red_channel_client_get_message_serial(RedChannelClient *rcc)
{
    return rcc->send_data.serial;
}
