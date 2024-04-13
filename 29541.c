void reds_on_main_agent_data(MainChannelClient *mcc, void *message, size_t size)
{
    VDIPortState *dev_state = &reds->agent_state;
    VDIChunkHeader *header;
    int res;

    res = agent_msg_filter_process_data(&reds->agent_state.write_filter,
                                        message, size);
    switch (res) {
    case AGENT_MSG_FILTER_OK:
        break;
    case AGENT_MSG_FILTER_DISCARD:
        return;
    case AGENT_MSG_FILTER_MONITORS_CONFIG:
        reds_on_main_agent_monitors_config(mcc, message, size);
        return;
    case AGENT_MSG_FILTER_PROTO_ERROR:
        red_channel_client_shutdown(main_channel_client_get_base(mcc));
        return;
    }

    spice_assert(reds->agent_state.recv_from_client_buf);
    spice_assert(message == reds->agent_state.recv_from_client_buf->buf + sizeof(VDIChunkHeader));
    header =  (VDIChunkHeader *)dev_state->recv_from_client_buf->buf;
    header->port = VDP_CLIENT_PORT;
    header->size = size;
    dev_state->recv_from_client_buf->buf_used = sizeof(VDIChunkHeader) + size;

    dev_state->recv_from_client_buf_pushed = TRUE;
    spice_char_device_write_buffer_add(reds->agent_state.base, dev_state->recv_from_client_buf);
}
