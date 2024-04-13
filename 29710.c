static void red_channel_client_send_migrate(RedChannelClient *rcc)
{
    SpiceMsgMigrate migrate;

    red_channel_client_init_send_data(rcc, SPICE_MSG_MIGRATE, NULL);
    migrate.flags = rcc->channel->migration_flags;
    spice_marshall_msg_migrate(rcc->send_data.marshaller, &migrate);
    if (rcc->channel->migration_flags & SPICE_MIGRATE_NEED_FLUSH) {
        rcc->wait_migrate_flush_mark = TRUE;
    }

    red_channel_client_begin_send_message(rcc);
}
