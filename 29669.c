static void red_channel_client_event(int fd, int event, void *data)
{
    RedChannelClient *rcc = (RedChannelClient *)data;

    red_channel_client_ref(rcc);
    if (event & SPICE_WATCH_EVENT_READ) {
        red_channel_client_receive(rcc);
    }
    if (event & SPICE_WATCH_EVENT_WRITE) {
        red_channel_client_push(rcc);
    }
    red_channel_client_unref(rcc);
}
