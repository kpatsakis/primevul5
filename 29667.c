void red_channel_client_disconnect(RedChannelClient *rcc)
{
    spice_printerr("%p (channel %p type %d id %d)", rcc, rcc->channel,
                                                rcc->channel->type, rcc->channel->id);
    if (rcc->dummy) {
        red_channel_client_disconnect_dummy(rcc);
        return;
    }
    if (!red_channel_client_is_connected(rcc)) {
        return;
    }
    red_channel_client_pipe_clear(rcc);
    if (rcc->stream->watch) {
        rcc->channel->core->watch_remove(rcc->stream->watch);
        rcc->stream->watch = NULL;
    }
    reds_stream_free(rcc->stream);
    rcc->stream = NULL;
    if (rcc->latency_monitor.timer) {
        rcc->channel->core->timer_remove(rcc->latency_monitor.timer);
        rcc->latency_monitor.timer = NULL;
    }
    red_channel_remove_client(rcc);
    rcc->channel->channel_cbs.on_disconnect(rcc);
}
