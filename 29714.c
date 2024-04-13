static void red_channel_client_set_migration_seamless(RedChannelClient *rcc)
{
    spice_assert(rcc->client->during_target_migrate && rcc->client->seamless_migrate);

    if (rcc->channel->migration_flags & SPICE_MIGRATE_NEED_DATA_TRANSFER) {
        rcc->wait_migrate_data = TRUE;
        rcc->client->num_migrated_channels++;
    }
    spice_debug("channel type %d id %d rcc %p wait data %d", rcc->channel->type, rcc->channel->id, rcc,
        rcc->wait_migrate_data);
}
