static void reds_migrate_channels_seamless(void)
{
    RedClient *client;

    /* seamless migration is supported for only one client for now */
    client = reds_get_client();
    red_client_migrate(client);
}
