static void red_channel_client_push_ping(RedChannelClient *rcc)
{
    spice_assert(rcc->latency_monitor.state == PING_STATE_NONE);
    rcc->latency_monitor.state = PING_STATE_WARMUP;
    rcc->latency_monitor.warmup_was_sent = FALSE;
    rcc->latency_monitor.id = rand();
    red_channel_client_pipe_add_type(rcc, PIPE_ITEM_TYPE_PING);
    red_channel_client_pipe_add_type(rcc, PIPE_ITEM_TYPE_PING);
}
