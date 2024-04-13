int red_channel_client_is_connected(RedChannelClient *rcc)
{
    if (!rcc->dummy) {
        return rcc->stream != NULL;
    } else {
        return rcc->dummy_connected;
    }
}
