static void red_channel_client_send_item(RedChannelClient *rcc, PipeItem *item)
{
    int handled = TRUE;

    spice_assert(red_channel_client_no_item_being_sent(rcc));
    red_channel_client_reset_send_data(rcc);
    switch (item->type) {
        case PIPE_ITEM_TYPE_SET_ACK:
            red_channel_client_send_set_ack(rcc);
            free(item);
            break;
        case PIPE_ITEM_TYPE_MIGRATE:
            red_channel_client_send_migrate(rcc);
            free(item);
            break;
        case PIPE_ITEM_TYPE_EMPTY_MSG:
            red_channel_client_send_empty_msg(rcc, item);
            free(item);
            break;
        case PIPE_ITEM_TYPE_PING:
            red_channel_client_send_ping(rcc);
            free(item);
            break;
        default:
            handled = FALSE;
    }
    if (!handled) {
        rcc->channel->channel_cbs.send_item(rcc, item);
    }
}
