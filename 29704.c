static void red_channel_client_restart_ping_timer(RedChannelClient *rcc)
{
    struct timespec ts;
    uint64_t passed, timeout;

    clock_gettime(CLOCK_MONOTONIC, &ts);

    passed = ts.tv_sec * 1000000000LL + ts.tv_nsec;
    passed = passed - rcc->latency_monitor.last_pong_time;
    passed /= 1000*1000;
    timeout = PING_TEST_IDLE_NET_TIMEOUT_MS;
    if (passed  < PING_TEST_TIMEOUT_MS) {
        timeout += PING_TEST_TIMEOUT_MS - passed;
    }

    red_channel_client_start_ping_timer(rcc, timeout);
}
