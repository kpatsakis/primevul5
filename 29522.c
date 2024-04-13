static int reds_link_mig_target_channels(RedClient *client)
{
    RedsMigTargetClient *mig_client;
    RingItem *item;

    spice_info("%p", client);
    mig_client = reds_mig_target_client_find(client);
    if (!mig_client) {
        spice_info("Error: mig target client was not found");
        return FALSE;
    }

    /* Each channel should check if we are during migration, and
     * act accordingly. */
    RING_FOREACH(item, &mig_client->pending_links) {
        RedsMigPendingLink *mig_link;
        RedChannel *channel;

        mig_link = SPICE_CONTAINEROF(item, RedsMigPendingLink, ring_link);
        channel = reds_find_channel(mig_link->link_msg->channel_type,
                                    mig_link->link_msg->channel_id);
        if (!channel) {
            spice_warning("client %p channel (%d, %d) (type, id) wasn't found",
                          client,
                          mig_link->link_msg->channel_type,
                          mig_link->link_msg->channel_id);
            continue;
        }
        reds_channel_do_link(channel, client, mig_link->link_msg, mig_link->stream);
    }

    reds_mig_target_client_free(mig_client);

    return TRUE;
}
