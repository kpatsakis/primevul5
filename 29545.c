void reds_on_main_channel_migrate(MainChannelClient *mcc)
{
    VDIPortState *agent_state = &reds->agent_state;
    uint32_t read_data_len;

    spice_assert(reds->num_clients == 1);

    if (agent_state->read_state != VDI_PORT_READ_STATE_READ_DATA) {
        return;
    }
    spice_assert(agent_state->current_read_buf->data &&
                 agent_state->receive_pos > agent_state->current_read_buf->data);
    read_data_len = agent_state->receive_pos - agent_state->current_read_buf->data;

    if (agent_state->read_filter.msg_data_to_read ||
        read_data_len > sizeof(VDAgentMessage)) { /* msg header has been read */
        VDIReadBuf *read_buf = agent_state->current_read_buf;

        spice_debug("push partial read %u (msg first chunk? %d)", read_data_len,
                    !agent_state->read_filter.msg_data_to_read);

        read_buf->len = read_data_len;
        if (vdi_port_read_buf_process(agent_state->vdi_chunk_header.port, read_buf)) {
            main_channel_client_push_agent_data(mcc,
                                                read_buf->data,
                                                read_buf->len,
                                                vdi_port_read_buf_release,
                                                read_buf);
        } else {
            vdi_port_read_buf_unref(read_buf);
        }

        spice_assert(agent_state->receive_len);
        agent_state->message_receive_len += agent_state->receive_len;
        agent_state->read_state = VDI_PORT_READ_STATE_GET_BUFF;
        agent_state->current_read_buf = NULL;
        agent_state->receive_pos = NULL;
    }
}
