RedChannel *red_channel_create_dummy(int size, uint32_t type, uint32_t id)
{
    RedChannel *channel;
    ClientCbs client_cbs = { NULL, };

    spice_assert(size >= sizeof(*channel));
    channel = spice_malloc0(size);
    channel->type = type;
    channel->id = id;
    channel->refs = 1;
    channel->core = &dummy_core;
    ring_init(&channel->clients);
    client_cbs.connect = red_channel_client_default_connect;
    client_cbs.disconnect = red_channel_client_default_disconnect;
    client_cbs.migrate = red_channel_client_default_migrate;

    red_channel_register_client_cbs(channel, &client_cbs);
    red_channel_set_common_cap(channel, SPICE_COMMON_CAP_MINI_HEADER);

    channel->thread_id = pthread_self();
    spice_debug("channel type %d id %d thread_id 0x%lx",
                channel->type, channel->id, channel->thread_id);

    channel->out_bytes_counter = 0;

    return channel;
}
