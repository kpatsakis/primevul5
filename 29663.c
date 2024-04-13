void red_channel_client_default_migrate(RedChannelClient *rcc)
{
    if (rcc->latency_monitor.timer) {
        red_channel_client_cancel_ping_timer(rcc);
        rcc->channel->core->timer_remove(rcc->latency_monitor.timer);
        rcc->latency_monitor.timer = NULL;
    }
    red_channel_client_pipe_add_type(rcc, PIPE_ITEM_TYPE_MIGRATE);
}
