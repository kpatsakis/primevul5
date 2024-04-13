static void red_channel_client_ping_timer(void *opaque)
{
    int so_unsent_size = 0;
    RedChannelClient *rcc = opaque;

    spice_assert(rcc->latency_monitor.state == PING_STATE_TIMER);
    red_channel_client_cancel_ping_timer(rcc);
    /* retrieving the occupied size of the socket's tcp snd buffer (unacked + unsent) */
    if (ioctl(rcc->stream->socket, TIOCOUTQ, &so_unsent_size) == -1) {
        spice_printerr("ioctl(TIOCOUTQ) failed, %s", strerror(errno));
    }
    if (so_unsent_size > 0) {
        /* tcp snd buffer is still occupied. rescheduling ping */
        red_channel_client_start_ping_timer(rcc, PING_TEST_IDLE_NET_TIMEOUT_MS);
    } else {
        red_channel_client_push_ping(rcc);
    }
}
