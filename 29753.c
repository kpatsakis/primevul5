void red_client_migrate(RedClient *client)
{
    RingItem *link, *next;
    RedChannelClient *rcc;

    spice_printerr("migrate client with #channels %d", client->channels_num);
    if (!pthread_equal(pthread_self(), client->thread_id)) {
        spice_warning("client->thread_id (0x%lx) != pthread_self (0x%lx)."
                      "If one of the threads is != io-thread && != vcpu-thread,"
                      " this might be a BUG",
                      client->thread_id, pthread_self());
    }
    RING_FOREACH_SAFE(link, next, &client->channels) {
        rcc = SPICE_CONTAINEROF(link, RedChannelClient, client_link);
        if (red_channel_client_is_connected(rcc)) {
            rcc->channel->client_cbs.migrate(rcc);
        }
    }
}
