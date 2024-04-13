static inline int red_channel_client_urgent_marshaller_is_active(RedChannelClient *rcc)
{
    return (rcc->send_data.marshaller == rcc->send_data.urgent.marshaller);
}
