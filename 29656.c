int red_channel_client_blocked(RedChannelClient *rcc)
{
    return rcc && rcc->send_data.blocked;
}
