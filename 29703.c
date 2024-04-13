static inline void red_channel_client_release_sent_item(RedChannelClient *rcc)
{
    if (rcc->send_data.item) {
        red_channel_client_release_item(rcc,
                                        rcc->send_data.item, TRUE);
        rcc->send_data.item = NULL;
    }
}
