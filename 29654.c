void red_channel_client_ack_set_client_window(RedChannelClient *rcc, int client_window)
{
    rcc->ack_data.client_window = client_window;
}
