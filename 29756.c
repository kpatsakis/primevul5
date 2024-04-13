void red_client_semi_seamless_migrate_complete(RedClient *client)
{
    RingItem *link;

    pthread_mutex_lock(&client->lock);
    if (!client->during_target_migrate || client->seamless_migrate) {
        spice_error("unexpected");
        pthread_mutex_unlock(&client->lock);
        return;
    }
    client->during_target_migrate = FALSE;
    RING_FOREACH(link, &client->channels) {
        RedChannelClient *rcc = SPICE_CONTAINEROF(link, RedChannelClient, client_link);

        if (rcc->latency_monitor.timer) {
            red_channel_client_start_ping_timer(rcc, PING_TEST_IDLE_NET_TIMEOUT_MS);
        }
    }
    pthread_mutex_unlock(&client->lock);
    reds_on_client_semi_seamless_migrate_complete(client);
}
