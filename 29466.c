static void init_vd_agent_resources(void)
{
    VDIPortState *state = &reds->agent_state;
    int i;

    ring_init(&state->read_bufs);
    agent_msg_filter_init(&state->write_filter, agent_copypaste,
                          agent_file_xfer, TRUE);
    agent_msg_filter_init(&state->read_filter, agent_copypaste,
                          agent_file_xfer, TRUE);

    state->read_state = VDI_PORT_READ_STATE_READ_HEADER;
    state->receive_pos = (uint8_t *)&state->vdi_chunk_header;
    state->receive_len = sizeof(state->vdi_chunk_header);

    for (i = 0; i < REDS_VDI_PORT_NUM_RECEIVE_BUFFS; i++) {
        VDIReadBuf *buf = spice_new0(VDIReadBuf, 1);
        ring_item_init(&buf->link);
        ring_add(&reds->agent_state.read_bufs, &buf->link);
    }
}
