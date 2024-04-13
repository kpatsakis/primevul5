void reds_client_disconnect(RedClient *client)
{
    RedsMigTargetClient *mig_client;

    if (exit_on_disconnect)
    {
        spice_info("Exiting server because of client disconnect.\n");
        exit(0);
    }

    if (!client || client->disconnecting) {
        spice_debug("client %p already during disconnection", client);
        return;
    }

    spice_info(NULL);
    /* disconnecting is set to prevent recursion because of the following:
     * main_channel_client_on_disconnect->
     *  reds_client_disconnect->red_client_destroy->main_channel...
     */
    client->disconnecting = TRUE;


    mig_client = reds_mig_target_client_find(client);
    if (mig_client) {
        reds_mig_target_client_free(mig_client);
    }

    if (reds->mig_wait_disconnect) {
        reds_mig_remove_wait_disconnect_client(client);
    }

    if (reds->agent_state.base) {
        /* note that vdagent might be NULL, if the vdagent was once
         * up and than was removed */
        if (spice_char_device_client_exists(reds->agent_state.base, client)) {
            spice_char_device_client_remove(reds->agent_state.base, client);
        }
    }

    ring_remove(&client->link);
    reds->num_clients--;
    red_client_destroy(client);

    if (reds->num_clients == 0) {
        /* Let the agent know the client is disconnected */
        if (reds->agent_state.base) {
            SpiceCharDeviceWriteBuffer *char_dev_buf;
            VDInternalBuf *internal_buf;
            uint32_t total_msg_size;

            total_msg_size = sizeof(VDIChunkHeader) + sizeof(VDAgentMessage);
            char_dev_buf = spice_char_device_write_buffer_get_server_no_token(
                               reds->agent_state.base, total_msg_size);
            char_dev_buf->buf_used = total_msg_size;
            internal_buf = (VDInternalBuf *)char_dev_buf->buf;
            internal_buf->chunk_header.port = VDP_SERVER_PORT;
            internal_buf->chunk_header.size = sizeof(VDAgentMessage);
            internal_buf->header.protocol = VD_AGENT_PROTOCOL;
            internal_buf->header.type = VD_AGENT_CLIENT_DISCONNECTED;
            internal_buf->header.opaque = 0;
            internal_buf->header.size = 0;

            spice_char_device_write_buffer_add(reds->agent_state.base,
                                               char_dev_buf);
        }

        /* Reset write filter to start with clean state on client reconnect */
        agent_msg_filter_init(&reds->agent_state.write_filter, agent_copypaste,
                              agent_file_xfer, TRUE);

        /* Throw away pending chunks from the current (if any) and future
         *  messages read from the agent */
        reds->agent_state.read_filter.result = AGENT_MSG_FILTER_DISCARD;
        reds->agent_state.read_filter.discard_all = TRUE;
        free(reds->agent_state.mig_data);
        reds->agent_state.mig_data = NULL;

        reds_mig_cleanup();
    }
}
