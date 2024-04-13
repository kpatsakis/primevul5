void red_channel_set_data(RedChannel *channel, void *data)
{
    spice_assert(channel);
    channel->data = data;
}
