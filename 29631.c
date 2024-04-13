static void vdi_port_send_tokens_to_client(RedClient *client, uint32_t tokens, void *opaque)
{
    main_channel_client_push_agent_tokens(red_client_get_main(client),
                                          tokens);
}
