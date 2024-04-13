void red_channel_client_set_message_serial(RedChannelClient *rcc, uint64_t serial)
{
    rcc->send_data.last_sent_serial = serial;
    rcc->send_data.serial = serial;
}
