static void reds_reset_vdp(void)
{
    VDIPortState *state = &reds->agent_state;
    SpiceCharDeviceInterface *sif;

    state->read_state = VDI_PORT_READ_STATE_READ_HEADER;
    state->receive_pos = (uint8_t *)&state->vdi_chunk_header;
    state->receive_len = sizeof(state->vdi_chunk_header);
    state->message_receive_len = 0;
    if (state->current_read_buf) {
        vdi_port_read_buf_unref(state->current_read_buf);
        state->current_read_buf = NULL;
    }
    /* Reset read filter to start with clean state when the agent reconnects */
    agent_msg_filter_init(&state->read_filter, agent_copypaste,
                          agent_file_xfer, TRUE);
    /* Throw away pending chunks from the current (if any) and future
     * messages written by the client.
     * TODO: client should clear its agent messages queue when the agent
     * is disconnect. Currently, when and agent gets disconnected and reconnected,
     * messeges that were directed to the previous instance of the agent continues
     * to be sent from the client. This TODO will require server, protocol, and client changes */
    state->write_filter.result = AGENT_MSG_FILTER_DISCARD;
    state->write_filter.discard_all = TRUE;
    state->client_agent_started = FALSE;

    /* reseting and not destroying the state as a workaround for a bad
     * tokens management in the vdagent protocol:
     *  The client tokens' are set only once, when the main channel is initialized.
     *  Instead, it would have been more appropriate to reset them upon AGEN_CONNECT.
     *  The client tokens are tracked as part of the SpiceCharDeviceClientState. Thus,
     *  in order to be backward compatible with the client, we need to track the tokens
     *  even if the agent is detached. We don't destroy the char_device state, and
     *  instead we just reset it.
     *  In addition, there used to be a misshandling of AGENT_TOKENS message in spice-gtk: it
     *  overrides the amount of tokens, instead of adding the given amount.
     */
    if (red_channel_test_remote_cap(&reds->main_channel->base,
                                    SPICE_MAIN_CAP_AGENT_CONNECTED_TOKENS)) {
        spice_char_device_state_destroy(state->base);
        state->base = NULL;
    } else {
        spice_char_device_reset(state->base);
    }

    sif = SPICE_CONTAINEROF(vdagent->base.sif, SpiceCharDeviceInterface, base);
    if (sif->state) {
        sif->state(vdagent, 0);
    }
}
