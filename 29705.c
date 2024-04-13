static void red_channel_client_restore_main_sender(RedChannelClient *rcc)
{
    spice_marshaller_reset(rcc->send_data.urgent.marshaller);
    rcc->send_data.marshaller = rcc->send_data.main.marshaller;
    rcc->send_data.header.data = rcc->send_data.main.header_data;
    if (!rcc->is_mini_header) {
        rcc->send_data.header.set_msg_serial(&rcc->send_data.header, rcc->send_data.serial);
    }
    rcc->send_data.item = rcc->send_data.main.item;
}
