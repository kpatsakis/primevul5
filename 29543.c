void reds_on_main_agent_start(MainChannelClient *mcc, uint32_t num_tokens)
{
    SpiceCharDeviceState *dev_state = reds->agent_state.base;
    RedChannelClient *rcc;

    if (!vdagent) {
        return;
    }
    spice_assert(vdagent->st && vdagent->st == dev_state);
    rcc = main_channel_client_get_base(mcc);
    reds->agent_state.client_agent_started = TRUE;
    /*
     * Note that in older releases, send_tokens were set to ~0 on both client
     * and server. The server ignored the client given tokens.
     * Thanks to that, when an old client is connected to a new server,
     * and vice versa, the sending from the server to the client won't have
     * flow control, but will have no other problem.
     */
    if (!spice_char_device_client_exists(dev_state, rcc->client)) {
        int client_added;

        client_added = spice_char_device_client_add(dev_state,
                                                    rcc->client,
                                                    TRUE, /* flow control */
                                                    REDS_VDI_PORT_NUM_RECEIVE_BUFFS,
                                                    REDS_AGENT_WINDOW_SIZE,
                                                    num_tokens,
                                                    red_channel_client_waits_for_migrate_data(rcc));

        if (!client_added) {
            spice_warning("failed to add client to agent");
            red_channel_client_shutdown(rcc);
            return;
        }
    } else {
        spice_char_device_send_to_client_tokens_set(dev_state,
                                                    rcc->client,
                                                    num_tokens);
    }
    reds->agent_state.write_filter.discard_all = FALSE;
}
