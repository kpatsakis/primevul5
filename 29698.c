void red_channel_client_push(RedChannelClient *rcc)
{
    PipeItem *pipe_item;

    if (!rcc->during_send) {
        rcc->during_send = TRUE;
    } else {
        return;
    }
    red_channel_client_ref(rcc);
    if (rcc->send_data.blocked) {
        red_channel_client_send(rcc);
    }

    if (!red_channel_client_no_item_being_sent(rcc) && !rcc->send_data.blocked) {
        rcc->send_data.blocked = TRUE;
        spice_printerr("ERROR: an item waiting to be sent and not blocked");
    }

    while ((pipe_item = red_channel_client_pipe_item_get(rcc))) {
        red_channel_client_send_item(rcc, pipe_item);
    }
    rcc->during_send = FALSE;
    red_channel_client_unref(rcc);
}
