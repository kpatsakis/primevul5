void red_channel_client_init_send_data(RedChannelClient *rcc, uint16_t msg_type, PipeItem *item)
{
    spice_assert(red_channel_client_no_item_being_sent(rcc));
    spice_assert(msg_type != 0);
    rcc->send_data.header.set_msg_type(&rcc->send_data.header, msg_type);
    rcc->send_data.item = item;
    if (item) {
        rcc->channel->channel_cbs.hold_item(rcc, item);
    }
}
