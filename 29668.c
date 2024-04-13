static void red_channel_client_disconnect_dummy(RedChannelClient *rcc)
{
    spice_assert(rcc->dummy);
    if (ring_item_is_linked(&rcc->channel_link)) {
        red_channel_remove_client(rcc);
    }
    rcc->dummy_connected = FALSE;
}
