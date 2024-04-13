int red_channel_client_get_roundtrip_ms(RedChannelClient *rcc)
{
    if (rcc->latency_monitor.roundtrip < 0) {
        return rcc->latency_monitor.roundtrip;
    }
    return rcc->latency_monitor.roundtrip / 1000 / 1000;
}
