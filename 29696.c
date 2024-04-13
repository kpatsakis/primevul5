void red_channel_client_pipe_remove_and_release(RedChannelClient *rcc,
                                                PipeItem *item)
{
    red_channel_client_pipe_remove(rcc, item);
    red_channel_client_release_item(rcc, item, FALSE);
}
