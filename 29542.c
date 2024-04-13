static void reds_on_main_agent_monitors_config(
        MainChannelClient *mcc, void *message, size_t size)
{
    VDAgentMessage *msg_header;
    VDAgentMonitorsConfig *monitors_config;
    RedsClientMonitorsConfig *cmc = &reds->client_monitors_config;

    cmc->buffer_size += size;
    cmc->buffer = realloc(cmc->buffer, cmc->buffer_size);
    spice_assert(cmc->buffer);
    cmc->mcc = mcc;
    memcpy(cmc->buffer + cmc->buffer_pos, message, size);
    cmc->buffer_pos += size;
    msg_header = (VDAgentMessage *)cmc->buffer;
    if (sizeof(VDAgentMessage) > cmc->buffer_size ||
            msg_header->size > cmc->buffer_size - sizeof(VDAgentMessage)) {
        spice_debug("not enough data yet. %d\n", cmc->buffer_size);
        return;
    }
    monitors_config = (VDAgentMonitorsConfig *)(cmc->buffer + sizeof(*msg_header));
    spice_debug("%s: %d\n", __func__, monitors_config->num_of_monitors);
    red_dispatcher_client_monitors_config(monitors_config);
    reds_client_monitors_config_cleanup();
}
