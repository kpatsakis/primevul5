void red_channel_register_client_cbs(RedChannel *channel, ClientCbs *client_cbs)
{
    spice_assert(client_cbs->connect || channel->type == SPICE_CHANNEL_MAIN);
    channel->client_cbs.connect = client_cbs->connect;

    if (client_cbs->disconnect) {
        channel->client_cbs.disconnect = client_cbs->disconnect;
    }

    if (client_cbs->migrate) {
        channel->client_cbs.migrate = client_cbs->migrate;
    }
}
