static void red_channel_client_cancel_ping_timer(RedChannelClient *rcc)
{
    if (!rcc->latency_monitor.timer) {
        return;
    }
    if (rcc->latency_monitor.state != PING_STATE_TIMER) {
        return;
    }

    rcc->channel->core->timer_cancel(rcc->latency_monitor.timer);
    rcc->latency_monitor.state = PING_STATE_NONE;
}
