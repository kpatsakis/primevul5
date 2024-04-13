static void red_channel_client_handle_pong(RedChannelClient *rcc, SpiceMsgPing *ping)
{
    uint64_t now;
    struct timespec ts;

    /* ignoring unexpected pongs, or post-migration pongs for pings that
     * started just before migration */
    if (ping->id != rcc->latency_monitor.id) {
        spice_warning("ping-id (%u)!= pong-id %u",
                      rcc->latency_monitor.id, ping->id);
        return;
    }

    clock_gettime(CLOCK_MONOTONIC, &ts);
    now =  ts.tv_sec * 1000000000LL + ts.tv_nsec;

    if (rcc->latency_monitor.state == PING_STATE_WARMUP) {
        rcc->latency_monitor.state = PING_STATE_LATENCY;
        return;
    } else if (rcc->latency_monitor.state != PING_STATE_LATENCY) {
        spice_warning("unexpected");
        return;
    }

    /* set TCO_NODELAY=0, in case we reverted it for the test*/
    if (!rcc->latency_monitor.tcp_nodelay) {
        int delay_val = 0;

        if (setsockopt(rcc->stream->socket, IPPROTO_TCP, TCP_NODELAY, &delay_val,
                       sizeof(delay_val)) == -1) {
            if (errno != ENOTSUP) {
                spice_warning("setsockopt failed, %s", strerror(errno));
            }
        }
    }

    /*
     * The real network latency shouldn't change during the connection. However,
     *  the measurements can be bigger than the real roundtrip due to other
     *  threads or processes that are utilizing the network. We update the roundtrip
     *  measurement with the minimal value we encountered till now.
     */
    if (rcc->latency_monitor.roundtrip < 0 ||
        now - ping->timestamp < rcc->latency_monitor.roundtrip) {
        rcc->latency_monitor.roundtrip = now - ping->timestamp;
        spice_debug("update roundtrip %.2f(ms)", rcc->latency_monitor.roundtrip/1000.0/1000.0);
    }

    rcc->latency_monitor.last_pong_time = now;
    rcc->latency_monitor.state = PING_STATE_NONE;
    red_channel_client_start_ping_timer(rcc, PING_TEST_TIMEOUT_MS);
}
