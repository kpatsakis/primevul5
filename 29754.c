RedClient *red_client_new(int migrated)
{
    RedClient *client;

    client = spice_malloc0(sizeof(RedClient));
    ring_init(&client->channels);
    pthread_mutex_init(&client->lock, NULL);
    client->thread_id = pthread_self();
    client->during_target_migrate = migrated;

    return client;
}
