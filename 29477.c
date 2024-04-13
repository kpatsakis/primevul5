static int reds_agent_state_restore(SpiceMigrateDataMain *mig_data)
{
    VDIPortState *agent_state = &reds->agent_state;
    uint32_t chunk_header_remaining;

    agent_state->vdi_chunk_header = mig_data->agent2client.chunk_header;
    spice_assert(mig_data->agent2client.chunk_header_size <= sizeof(VDIChunkHeader));
    chunk_header_remaining = sizeof(VDIChunkHeader) - mig_data->agent2client.chunk_header_size;
    if (chunk_header_remaining) {
        agent_state->read_state = VDI_PORT_READ_STATE_READ_HEADER;
        agent_state->receive_pos = (uint8_t *)&agent_state->vdi_chunk_header +
            mig_data->agent2client.chunk_header_size;
        agent_state->receive_len = chunk_header_remaining;
    } else {
        agent_state->message_receive_len = agent_state->vdi_chunk_header.size;
    }

    if (!mig_data->agent2client.msg_header_done) {
        uint8_t *partial_msg_header;

        if (!chunk_header_remaining) {
            uint32_t cur_buf_size;

            agent_state->read_state = VDI_PORT_READ_STATE_READ_DATA;
            agent_state->current_read_buf = vdi_port_read_buf_get();
            spice_assert(agent_state->current_read_buf);
            partial_msg_header = (uint8_t *)mig_data + mig_data->agent2client.msg_header_ptr -
                sizeof(SpiceMiniDataHeader);
            memcpy(agent_state->current_read_buf->data,
                   partial_msg_header,
                   mig_data->agent2client.msg_header_partial_len);
            agent_state->receive_pos = agent_state->current_read_buf->data +
                                      mig_data->agent2client.msg_header_partial_len;
            cur_buf_size = sizeof(agent_state->current_read_buf->data) -
                           mig_data->agent2client.msg_header_partial_len;
            agent_state->receive_len = MIN(agent_state->message_receive_len, cur_buf_size);
            agent_state->current_read_buf->len = agent_state->receive_len +
                                                 mig_data->agent2client.msg_header_partial_len;
            agent_state->message_receive_len -= agent_state->receive_len;
        } else {
            spice_assert(mig_data->agent2client.msg_header_partial_len == 0);
        }
    } else {
            agent_state->read_state = VDI_PORT_READ_STATE_GET_BUFF;
            agent_state->current_read_buf = NULL;
            agent_state->receive_pos = NULL;
            agent_state->read_filter.msg_data_to_read = mig_data->agent2client.msg_remaining;
            agent_state->read_filter.result = mig_data->agent2client.msg_filter_result;
    }

    agent_state->read_filter.discard_all = FALSE;
    agent_state->write_filter.discard_all = !mig_data->client_agent_started;
    agent_state->client_agent_started = mig_data->client_agent_started;

    agent_state->write_filter.msg_data_to_read = mig_data->client2agent.msg_remaining;
    agent_state->write_filter.result = mig_data->client2agent.msg_filter_result;

    spice_debug("to agent filter: discard all %d, wait_msg %u, msg_filter_result %d",
                agent_state->write_filter.discard_all,
                agent_state->write_filter.msg_data_to_read,
                 agent_state->write_filter.result);
    spice_debug("from agent filter: discard all %d, wait_msg %u, msg_filter_result %d",
                agent_state->read_filter.discard_all,
                agent_state->read_filter.msg_data_to_read,
                 agent_state->read_filter.result);
    return spice_char_device_state_restore(agent_state->base, &mig_data->agent_base);
}
