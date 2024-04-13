static void red_channel_handle_migrate_data(RedChannelClient *rcc, uint32_t size, void *message)
{
    spice_debug("channel type %d id %d rcc %p size %u",
                rcc->channel->type, rcc->channel->id, rcc, size);
    if (!rcc->channel->channel_cbs.handle_migrate_data) {
        return;
    }
    if (!red_channel_client_waits_for_migrate_data(rcc)) {
        spice_channel_client_error(rcc, "unexpected");
        return;
    }
    if (rcc->channel->channel_cbs.handle_migrate_data_get_serial) {
        red_channel_client_set_message_serial(rcc,
            rcc->channel->channel_cbs.handle_migrate_data_get_serial(rcc, size, message));
    }
    if (!rcc->channel->channel_cbs.handle_migrate_data(rcc, size, message)) {
        spice_channel_client_error(rcc, "handle_migrate_data failed");
        return;
    }
    red_channel_client_seamless_migration_done(rcc);
}
