void red_channel_pipes_new_add_push(RedChannel *channel,
                              new_pipe_item_t creator, void *data)
{
    red_channel_pipes_create_batch(channel, creator, data,
                                     red_channel_client_pipe_add);
    red_channel_push(channel);
}
