RedChannel *red_channel_create(int size,
                               SpiceCoreInterface *core,
                               uint32_t type, uint32_t id,
                               int handle_acks,
                               channel_handle_message_proc handle_message,
                               ChannelCbs *channel_cbs,
                               uint32_t migration_flags)
{
    RedChannel *channel;
    ClientCbs client_cbs = { NULL, };

    spice_assert(size >= sizeof(*channel));
    spice_assert(channel_cbs->config_socket && channel_cbs->on_disconnect && handle_message &&
           channel_cbs->alloc_recv_buf && channel_cbs->release_item);
    spice_assert(channel_cbs->handle_migrate_data ||
                 !(migration_flags & SPICE_MIGRATE_NEED_DATA_TRANSFER));
    channel = spice_malloc0(size);
    channel->type = type;
    channel->id = id;
    channel->refs = 1;
    channel->handle_acks = handle_acks;
    channel->migration_flags = migration_flags;
    memcpy(&channel->channel_cbs, channel_cbs, sizeof(ChannelCbs));

    channel->core = core;
    ring_init(&channel->clients);

    channel->incoming_cb.alloc_msg_buf = (alloc_msg_recv_buf_proc)channel_cbs->alloc_recv_buf;
    channel->incoming_cb.release_msg_buf = (release_msg_recv_buf_proc)channel_cbs->release_recv_buf;
    channel->incoming_cb.handle_message = (handle_message_proc)handle_message;
    channel->incoming_cb.on_error =
        (on_incoming_error_proc)red_channel_client_default_peer_on_error;
    channel->outgoing_cb.get_msg_size = red_channel_client_peer_get_out_msg_size;
    channel->outgoing_cb.prepare = red_channel_client_peer_prepare_out_msg;
    channel->outgoing_cb.on_block = red_channel_client_peer_on_out_block;
    channel->outgoing_cb.on_error =
        (on_outgoing_error_proc)red_channel_client_default_peer_on_error;
    channel->outgoing_cb.on_msg_done = red_channel_peer_on_out_msg_done;
    channel->outgoing_cb.on_output = red_channel_client_on_output;

    client_cbs.connect = red_channel_client_default_connect;
    client_cbs.disconnect = red_channel_client_default_disconnect;
    client_cbs.migrate = red_channel_client_default_migrate;

    red_channel_register_client_cbs(channel, &client_cbs);
    red_channel_set_common_cap(channel, SPICE_COMMON_CAP_MINI_HEADER);

    channel->thread_id = pthread_self();

    channel->out_bytes_counter = 0;

    spice_debug("channel type %d id %d thread_id 0x%lx",
                channel->type, channel->id, channel->thread_id);
    return channel;
}
