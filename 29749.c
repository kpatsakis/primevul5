void red_client_destroy(RedClient *client)
{
    RingItem *link, *next;
    RedChannelClient *rcc;

    spice_printerr("destroy client with #channels %d", client->channels_num);
    if (!pthread_equal(pthread_self(), client->thread_id)) {
        spice_warning("client->thread_id (0x%lx) != pthread_self (0x%lx)."
                      "If one of the threads is != io-thread && != vcpu-thread,"
                      " this might be a BUG",
                      client->thread_id,
                      pthread_self());
    }
    RING_FOREACH_SAFE(link, next, &client->channels) {
        rcc = SPICE_CONTAINEROF(link, RedChannelClient, client_link);
        rcc->destroying = 1;
        rcc->channel->client_cbs.disconnect(rcc);
        spice_assert(ring_is_empty(&rcc->pipe));
        spice_assert(rcc->pipe_size == 0);
        spice_assert(rcc->send_data.size == 0);
        red_channel_client_destroy(rcc);
    }

    pthread_mutex_destroy(&client->lock);
    free(client);
}
