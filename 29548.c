int reds_on_migrate_dst_set_seamless(MainChannelClient *mcc, uint32_t src_version)
{
    /* seamless migration is not supported with multiple clients*/
    if (reds->allow_multiple_clients  || src_version > SPICE_MIGRATION_PROTOCOL_VERSION) {
        reds->dst_do_seamless_migrate = FALSE;
    } else {
        RedChannelClient *rcc = main_channel_client_get_base(mcc);

        red_client_set_migration_seamless(rcc->client);
        /* linking all the channels that have been connected before migration handshake */
        reds->dst_do_seamless_migrate = reds_link_mig_target_channels(rcc->client);
    }
    return reds->dst_do_seamless_migrate;
}
