static int red_channel_client_pre_create_validate(RedChannel *channel, RedClient  *client)
{
    if (red_client_get_channel(client, channel->type, channel->id)) {
        spice_printerr("Error client %p: duplicate channel type %d id %d",
                       client, channel->type, channel->id);
        return FALSE;
    }
    return TRUE;
}
