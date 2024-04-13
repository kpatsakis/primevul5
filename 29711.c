static void red_channel_client_send_ping(RedChannelClient *rcc)
{
    SpiceMsgPing ping;
    struct timespec ts;

    if (!rcc->latency_monitor.warmup_was_sent) { // latency test start
        int delay_val;
        socklen_t opt_size = sizeof(delay_val);

        rcc->latency_monitor.warmup_was_sent = TRUE;
        /*
         * When testing latency, TCP_NODELAY must be switched on, otherwise,
         * sending the ping message is delayed by Nagle algorithm, and the
         * roundtrip measurment is less accurate (bigger).
         */
        rcc->latency_monitor.tcp_nodelay = 1;
        if (getsockopt(rcc->stream->socket, IPPROTO_TCP, TCP_NODELAY, &delay_val,
                       &opt_size) == -1) {
            spice_warning("getsockopt failed, %s", strerror(errno));
        }  else {
            rcc->latency_monitor.tcp_nodelay = delay_val;
            if (!delay_val) {
                delay_val = 1;
                if (setsockopt(rcc->stream->socket, IPPROTO_TCP, TCP_NODELAY, &delay_val,
                               sizeof(delay_val)) == -1) {
                   if (errno != ENOTSUP) {
                        spice_warning("setsockopt failed, %s", strerror(errno));
                    }
                }
            }
        }
    }

    red_channel_client_init_send_data(rcc, SPICE_MSG_PING, NULL);
    ping.id = rcc->latency_monitor.id;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    ping.timestamp = ts.tv_sec * 1000000000LL + ts.tv_nsec;
    spice_marshall_msg_ping(rcc->send_data.marshaller, &ping);
    red_channel_client_begin_send_message(rcc);
}
