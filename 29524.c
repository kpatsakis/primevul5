void reds_marshall_migrate_data(SpiceMarshaller *m)
{
    SpiceMigrateDataMain mig_data;
    VDIPortState *agent_state = &reds->agent_state;
    SpiceMarshaller *m2;

    memset(&mig_data, 0, sizeof(mig_data));
    spice_marshaller_add_uint32(m, SPICE_MIGRATE_DATA_MAIN_MAGIC);
    spice_marshaller_add_uint32(m, SPICE_MIGRATE_DATA_MAIN_VERSION);

    if (!vdagent) {
        uint8_t *null_agent_mig_data;

        spice_assert(!agent_state->base); /* MSG_AGENT_CONNECTED_TOKENS is supported by the client
                                             (see spice_server_migrate_connect), so SpiceCharDeviceState
                                             is destroyed when the agent is disconnected and
                                             there is no need to track the client tokens
                                             (see reds_reset_vdp) */
        spice_char_device_state_migrate_data_marshall_empty(m);
        null_agent_mig_data = spice_marshaller_reserve_space(m,
                                                             sizeof(SpiceMigrateDataMain) -
                                                             sizeof(SpiceMigrateDataCharDevice));
        memset(null_agent_mig_data,
               0,
               sizeof(SpiceMigrateDataMain) - sizeof(SpiceMigrateDataCharDevice));
        return;
    }

    spice_char_device_state_migrate_data_marshall(reds->agent_state.base, m);
    spice_marshaller_add_uint8(m, reds->agent_state.client_agent_started);

    mig_data.agent2client.chunk_header = agent_state->vdi_chunk_header;

    /* agent to client partial msg */
    if (agent_state->read_state == VDI_PORT_READ_STATE_READ_HEADER) {
        mig_data.agent2client.chunk_header_size = agent_state->receive_pos -
            (uint8_t *)&agent_state->vdi_chunk_header;

        mig_data.agent2client.msg_header_done = FALSE;
        mig_data.agent2client.msg_header_partial_len = 0;
        spice_assert(!agent_state->read_filter.msg_data_to_read);
    } else {
        mig_data.agent2client.chunk_header_size = sizeof(VDIChunkHeader);
        mig_data.agent2client.chunk_header.size = agent_state->message_receive_len;
        if (agent_state->read_state == VDI_PORT_READ_STATE_READ_DATA) {
            /* in the middle of reading the message header (see reds_on_main_channel_migrate) */
            mig_data.agent2client.msg_header_done = FALSE;
            mig_data.agent2client.msg_header_partial_len =
                agent_state->receive_pos - agent_state->current_read_buf->data;
            spice_assert(mig_data.agent2client.msg_header_partial_len < sizeof(VDAgentMessage));
            spice_assert(!agent_state->read_filter.msg_data_to_read);
        } else {
            mig_data.agent2client.msg_header_done =  TRUE;
            mig_data.agent2client.msg_remaining = agent_state->read_filter.msg_data_to_read;
            mig_data.agent2client.msg_filter_result = agent_state->read_filter.result;
        }
    }
    spice_marshaller_add_uint32(m, mig_data.agent2client.chunk_header_size);
    spice_marshaller_add(m,
                         (uint8_t *)&mig_data.agent2client.chunk_header,
                         sizeof(VDIChunkHeader));
    spice_marshaller_add_uint8(m, mig_data.agent2client.msg_header_done);
    spice_marshaller_add_uint32(m, mig_data.agent2client.msg_header_partial_len);
    m2 = spice_marshaller_get_ptr_submarshaller(m, 0);
    spice_marshaller_add(m2, agent_state->current_read_buf->data,
                         mig_data.agent2client.msg_header_partial_len);
    spice_marshaller_add_uint32(m, mig_data.agent2client.msg_remaining);
    spice_marshaller_add_uint8(m, mig_data.agent2client.msg_filter_result);

    mig_data.client2agent.msg_remaining = agent_state->write_filter.msg_data_to_read;
    mig_data.client2agent.msg_filter_result = agent_state->write_filter.result;
    spice_marshaller_add_uint32(m, mig_data.client2agent.msg_remaining);
    spice_marshaller_add_uint8(m, mig_data.client2agent.msg_filter_result);
    spice_debug("from agent filter: discard all %d, wait_msg %u, msg_filter_result %d",
                agent_state->read_filter.discard_all,
                agent_state->read_filter.msg_data_to_read,
                 agent_state->read_filter.result);
    spice_debug("to agent filter: discard all %d, wait_msg %u, msg_filter_result %d",
                agent_state->write_filter.discard_all,
                agent_state->write_filter.msg_data_to_read,
                 agent_state->write_filter.result);
}
