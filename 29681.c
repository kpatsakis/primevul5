static int red_channel_client_peer_get_out_msg_size(void *opaque)
{
    RedChannelClient *rcc = (RedChannelClient *)opaque;

    return rcc->send_data.size;
}
