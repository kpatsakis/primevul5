static void red_channel_client_release_item(RedChannelClient *rcc, PipeItem *item, int item_pushed)
{
    int handled = TRUE;

    switch (item->type) {
        case PIPE_ITEM_TYPE_SET_ACK:
        case PIPE_ITEM_TYPE_EMPTY_MSG:
        case PIPE_ITEM_TYPE_MIGRATE:
        case PIPE_ITEM_TYPE_PING:
            free(item);
            break;
        default:
            handled = FALSE;
    }
    if (!handled) {
        rcc->channel->channel_cbs.release_item(rcc, item, item_pushed);
    }
}
