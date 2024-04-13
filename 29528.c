static void reds_mig_fill_wait_disconnect(void)
{
    RingItem *client_item;

    spice_assert(reds->num_clients > 0);
    /* tracking the clients, in order to ignore disconnection
     * of clients that got connected to the src after migration completion.*/
    RING_FOREACH(client_item, &reds->clients) {
        RedClient *client = SPICE_CONTAINEROF(client_item, RedClient, link);
        RedsMigWaitDisconnectClient *wait_client;

        wait_client = spice_new0(RedsMigWaitDisconnectClient, 1);
        wait_client->client = client;
        ring_add(&reds->mig_wait_disconnect_clients, &wait_client->link);
    }
    reds->mig_wait_disconnect = TRUE;
    core->timer_start(reds->mig_timer, MIGRATE_TIMEOUT);
}
