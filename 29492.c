uint8_t *reds_get_agent_data_buffer(MainChannelClient *mcc, size_t size)
{
    VDIPortState *dev_state = &reds->agent_state;
    RedClient *client;

    if (!dev_state->client_agent_started) {
        /*
         * agent got disconnected, and possibly got reconnected, but we still can receive
         * msgs that are addressed to the agent's old instance, in case they were
         * sent by the client before it received the AGENT_DISCONNECTED msg.
         * In such case, we will receive and discard the msgs (reds_reset_vdp takes care
         * of setting state->write_filter.result = AGENT_MSG_FILTER_DISCARD).
         */
        return spice_malloc(size);
    }

    spice_assert(dev_state->recv_from_client_buf == NULL);
    client = main_channel_client_get_base(mcc)->client;
    dev_state->recv_from_client_buf = spice_char_device_write_buffer_get(dev_state->base,
                                                                         client,
                                                                         size + sizeof(VDIChunkHeader));
    dev_state->recv_from_client_buf_pushed = FALSE;
    return dev_state->recv_from_client_buf->buf + sizeof(VDIChunkHeader);
}
