RedChannelClient *red_channel_client_create(int size, RedChannel *channel, RedClient  *client,
                                            RedsStream *stream,
                                            int monitor_latency,
                                            int num_common_caps, uint32_t *common_caps,
                                            int num_caps, uint32_t *caps)
{
    RedChannelClient *rcc = NULL;

    pthread_mutex_lock(&client->lock);
    if (!red_channel_client_pre_create_validate(channel, client)) {
        goto error;
    }
    spice_assert(stream && channel && size >= sizeof(RedChannelClient));
    rcc = spice_malloc0(size);
    rcc->stream = stream;
    rcc->channel = channel;
    rcc->client = client;
    rcc->refs = 1;
    rcc->ack_data.messages_window = ~0;  // blocks send message (maybe use send_data.blocked +
    rcc->ack_data.client_generation = ~0;
    rcc->ack_data.client_window = CLIENT_ACK_WINDOW;
    rcc->send_data.main.marshaller = spice_marshaller_new();
    rcc->send_data.urgent.marshaller = spice_marshaller_new();

    rcc->send_data.marshaller = rcc->send_data.main.marshaller;

    rcc->incoming.opaque = rcc;
    rcc->incoming.cb = &channel->incoming_cb;

    rcc->outgoing.opaque = rcc;
    rcc->outgoing.cb = &channel->outgoing_cb;
    rcc->outgoing.pos = 0;
    rcc->outgoing.size = 0;

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

    if (!channel->channel_cbs.config_socket(rcc)) {
        goto error;
    }

    ring_init(&rcc->pipe);
    rcc->pipe_size = 0;

    stream->watch = channel->core->watch_add(stream->socket,
                                           SPICE_WATCH_EVENT_READ,
                                           red_channel_client_event, rcc);
    rcc->id = channel->clients_num;
    red_channel_add_client(channel, rcc);
    red_client_add_channel(client, rcc);
    red_channel_ref(channel);
    pthread_mutex_unlock(&client->lock);

    if (monitor_latency) {
        rcc->latency_monitor.timer = channel->core->timer_add(
            red_channel_client_ping_timer, rcc);
        if (!client->during_target_migrate) {
            red_channel_client_start_ping_timer(rcc, PING_TEST_IDLE_NET_TIMEOUT_MS);
        }
        rcc->latency_monitor.roundtrip = -1;
    }

    return rcc;
error:
    free(rcc);
    reds_stream_free(stream);
    pthread_mutex_unlock(&client->lock);
    return NULL;
}
