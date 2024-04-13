static void reds_handle_other_links(RedLinkInfo *link)
{
    RedChannel *channel;
    RedClient *client = NULL;
    SpiceLinkMess *link_mess;
    RedsMigTargetClient *mig_client;

    link_mess = link->link_mess;
    if (reds->main_channel) {
        client = main_channel_get_client_by_link_id(reds->main_channel,
                                                    link_mess->connection_id);
    }

    if (!client) {
        reds_send_link_result(link, SPICE_LINK_ERR_BAD_CONNECTION_ID);
        reds_link_free(link);
        return;
    }

    if (!(channel = reds_find_channel(link_mess->channel_type,
                                      link_mess->channel_id))) {
        reds_send_link_result(link, SPICE_LINK_ERR_CHANNEL_NOT_AVAILABLE);
        reds_link_free(link);
        return;
    }

    reds_send_link_result(link, SPICE_LINK_ERR_OK);
    reds_info_new_channel(link, link_mess->connection_id);
    reds_stream_remove_watch(link->stream);

    mig_client = reds_mig_target_client_find(client);
    /*
     * In semi-seamless migration, we activate the channels only
     * after migration is completed. Since, the session starts almost from
     * scratch we don't mind if we skip some messages in between the src session end and
     * dst session start.
     * In seamless migration, in order to keep the continuousness of the session, and
     * in order not to lose any data, we activate the target channels before
     * migration completes, as soon as we receive SPICE_MSGC_MAIN_MIGRATE_DST_DO_SEAMLESS.
     * If a channel connects before receiving SPICE_MSGC_MAIN_MIGRATE_DST_DO_SEAMLESS,
     * reds_on_migrate_dst_set_seamless will take care of activating it */
    if (red_client_during_migrate_at_target(client) && !reds->dst_do_seamless_migrate) {
        spice_assert(mig_client);
        reds_mig_target_client_add_pending_link(mig_client, link_mess, link->stream);
    } else {
        spice_assert(!mig_client);
        reds_channel_do_link(channel, client, link_mess, link->stream);
        free(link_mess);
    }
    link->stream = NULL;
    link->link_mess = NULL;
    reds_link_free(link);
}
