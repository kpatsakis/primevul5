static void red_channel_client_start_ping_timer(RedChannelClient *rcc, uint32_t timeout)
{
    if (!rcc->latency_monitor.timer) {
        return;
    }
    if (rcc->latency_monitor.state != PING_STATE_NONE) {
        return;
    }
    rcc->latency_monitor.state = PING_STATE_TIMER;
    rcc->channel->core->timer_start(rcc->latency_monitor.timer, timeout);
}
