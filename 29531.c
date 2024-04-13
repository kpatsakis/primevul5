static void reds_mig_remove_wait_disconnect_client(RedClient *client)
{
    RingItem *wait_client_item;

    RING_FOREACH(wait_client_item, &reds->mig_wait_disconnect_clients) {
        RedsMigWaitDisconnectClient *wait_client;

        wait_client = SPICE_CONTAINEROF(wait_client_item, RedsMigWaitDisconnectClient, link);
        if (wait_client->client == client) {
            ring_remove(wait_client_item);
            free(wait_client);
            if (ring_is_empty(&reds->mig_wait_disconnect_clients)) {
                reds_mig_cleanup();
            }
            return;
        }
    }
    spice_warning("client not found %p", client);
}
