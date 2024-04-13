static void vdi_port_send_msg_to_client(SpiceCharDeviceMsgToClient *msg,
                                        RedClient *client,
                                        void *opaque)
{
    VDIReadBuf *agent_data_buf = msg;

    main_channel_client_push_agent_data(red_client_get_main(client),
                                        agent_data_buf->data,
                                        agent_data_buf->len,
                                        vdi_port_read_buf_release,
                                        vdi_port_read_buf_ref(agent_data_buf));
}
