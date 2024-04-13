int red_channel_client_no_item_being_sent(RedChannelClient *rcc)
{
    return !rcc || (rcc->send_data.size == 0);
}
