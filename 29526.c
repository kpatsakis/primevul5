static void reds_mig_cleanup_wait_disconnect(void)
{
    RingItem *wait_client_item;

    while ((wait_client_item = ring_get_tail(&reds->mig_wait_disconnect_clients))) {
        RedsMigWaitDisconnectClient *wait_client;

        wait_client = SPICE_CONTAINEROF(wait_client_item, RedsMigWaitDisconnectClient, link);
        ring_remove(wait_client_item);
        free(wait_client);
    }
    reds->mig_wait_disconnect = FALSE;
}
