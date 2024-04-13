RedChannelClient *red_channel_client_create_dummy(int size,
                                                  RedChannel *channel,
                                                  RedClient  *client,
                                                  int num_common_caps, uint32_t *common_caps,
                                                  int num_caps, uint32_t *caps)
{
    RedChannelClient *rcc = NULL;

    spice_assert(size >= sizeof(RedChannelClient));

    pthread_mutex_lock(&client->lock);
    if (!red_channel_client_pre_create_validate(channel, client)) {
        goto error;
    }
    rcc = spice_malloc0(size);
    rcc->refs = 1;
    rcc->client = client;
    rcc->channel = channel;
    red_channel_ref(channel);
    red_channel_client_set_remote_caps(rcc, num_common_caps, common_caps, num_caps, caps);
    if (red_channel_client_test_remote_common_cap(rcc, SPICE_COMMON_CAP_MINI_HEADER)) {
        rcc->incoming.header = mini_header_wrapper;
        rcc->send_data.header = mini_header_wrapper;
        rcc->is_mini_header = TRUE;
    } else {
        rcc->incoming.header = full_header_wrapper;
        rcc->send_data.header = full_header_wrapper;
        rcc->is_mini_header = FALSE;
    }

    rcc->incoming.header.data = rcc->incoming.header_buf;
    rcc->incoming.serial = 1;
    ring_init(&rcc->pipe);

    rcc->dummy = TRUE;
    rcc->dummy_connected = TRUE;
    red_channel_add_client(channel, rcc);
    red_client_add_channel(client, rcc);
    pthread_mutex_unlock(&client->lock);
    return rcc;
error:
    pthread_mutex_unlock(&client->lock);
    return NULL;
}
