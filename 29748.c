static void red_client_add_channel(RedClient *client, RedChannelClient *rcc)
{
    spice_assert(rcc && client);
    ring_add(&client->channels, &rcc->client_link);
    if (client->during_target_migrate && client->seamless_migrate) {
        red_channel_client_set_migration_seamless(rcc);
    }
    client->channels_num++;
}
