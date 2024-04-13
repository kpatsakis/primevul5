static void red_client_remove_channel(RedChannelClient *rcc)
{
    pthread_mutex_lock(&rcc->client->lock);
    ring_remove(&rcc->client_link);
    rcc->client->channels_num--;
    pthread_mutex_unlock(&rcc->client->lock);
}
