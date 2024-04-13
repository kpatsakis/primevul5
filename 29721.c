static inline int red_channel_client_waiting_for_ack(RedChannelClient *rcc)
{
    return (rcc->channel->handle_acks &&
            (rcc->ack_data.messages_window > rcc->ack_data.client_window * 2));
}
