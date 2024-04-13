void reds_on_main_agent_tokens(MainChannelClient *mcc, uint32_t num_tokens)
{
    if (!vdagent) {
        return;
    }
    spice_assert(vdagent->st);
    spice_char_device_send_to_client_tokens_add(vdagent->st,
                                                main_channel_client_get_base(mcc)->client,
                                                num_tokens);
}
