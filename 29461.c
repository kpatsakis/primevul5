static SpiceCharDeviceState *attach_to_red_agent(SpiceCharDeviceInstance *sin)
{
    VDIPortState *state = &reds->agent_state;
    SpiceCharDeviceInterface *sif;
    SpiceCharDeviceCallbacks char_dev_state_cbs;

    if (!state->base) {
        char_dev_state_cbs.read_one_msg_from_device = vdi_port_read_one_msg_from_device;
        char_dev_state_cbs.ref_msg_to_client = vdi_port_ref_msg_to_client;
        char_dev_state_cbs.unref_msg_to_client = vdi_port_unref_msg_to_client;
        char_dev_state_cbs.send_msg_to_client = vdi_port_send_msg_to_client;
        char_dev_state_cbs.send_tokens_to_client = vdi_port_send_tokens_to_client;
        char_dev_state_cbs.remove_client = vdi_port_remove_client;
        char_dev_state_cbs.on_free_self_token = vdi_port_on_free_self_token;

        state->base = spice_char_device_state_create(sin,
                                                     REDS_TOKENS_TO_SEND,
                                                     REDS_NUM_INTERNAL_AGENT_MESSAGES,
                                                     &char_dev_state_cbs,
                                                     NULL);
    } else {
        spice_char_device_state_reset_dev_instance(state->base, sin);
    }

    vdagent = sin;
    reds_update_mouse_mode();

    sif = SPICE_CONTAINEROF(vdagent->base.sif, SpiceCharDeviceInterface, base);
    if (sif->state) {
        sif->state(vdagent, 1);
    }

    if (!reds_main_channel_connected()) {
        return state->base;
    }

    state->read_filter.discard_all = FALSE;
    reds->agent_state.plug_generation++;

    if (reds->agent_state.mig_data) {
        spice_assert(reds->agent_state.plug_generation == 1);
        reds_agent_state_restore(reds->agent_state.mig_data);
        free(reds->agent_state.mig_data);
        reds->agent_state.mig_data = NULL;
    } else if (!red_channel_waits_for_migrate_data(&reds->main_channel->base)) {
        /* we will assoicate the client with the char device, upon reds_on_main_agent_start,
         * in response to MSGC_AGENT_START */
        main_channel_push_agent_connected(reds->main_channel);
    } else {
       spice_debug("waiting for migration data");
        if (!spice_char_device_client_exists(reds->agent_state.base, reds_get_client())) {
            int client_added;

            client_added = spice_char_device_client_add(reds->agent_state.base,
                                                        reds_get_client(),
                                                        TRUE, /* flow control */
                                                        REDS_VDI_PORT_NUM_RECEIVE_BUFFS,
                                                        REDS_AGENT_WINDOW_SIZE,
                                                        ~0,
                                                        TRUE);

            if (!client_added) {
                spice_warning("failed to add client to agent");
                reds_disconnect();
            }

        }
    }
    return state->base;
}
