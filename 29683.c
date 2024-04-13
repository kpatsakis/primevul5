static void red_channel_client_peer_prepare_out_msg(
    void *opaque, struct iovec *vec, int *vec_size, int pos)
{
    RedChannelClient *rcc = (RedChannelClient *)opaque;

    *vec_size = spice_marshaller_fill_iovec(rcc->send_data.marshaller,
                                            vec, IOV_MAX, pos);
}
