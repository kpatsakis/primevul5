static RedChannelClient *red_client_get_channel(RedClient *client, int type, int id)
{
    RingItem *link;
    RedChannelClient *rcc;
    RedChannelClient *ret = NULL;

    RING_FOREACH(link, &client->channels) {
        rcc = SPICE_CONTAINEROF(link, RedChannelClient, client_link);
        if (rcc->channel->type == type && rcc->channel->id == id) {
            ret = rcc;
            break;
        }
    }
    return ret;
}
