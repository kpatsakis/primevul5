SPICE_GNUC_VISIBLE int spice_server_migrate_connect(SpiceServer *s, const char* dest,
                                                    int port, int secure_port,
                                                    const char* cert_subject)
{
    SpiceMigrateInterface *sif;
    int try_seamless;

    spice_info(NULL);
    spice_assert(migration_interface);
    spice_assert(reds == s);

    if (reds->expect_migrate) {
        spice_info("consecutive calls without migration. Canceling previous call");
        main_channel_migrate_src_complete(reds->main_channel, FALSE);
    }

    sif = SPICE_CONTAINEROF(migration_interface->base.sif, SpiceMigrateInterface, base);

    if (!reds_set_migration_dest_info(dest, port, secure_port, cert_subject)) {
        sif->migrate_connect_complete(migration_interface);
        return -1;
    }

    reds->expect_migrate = TRUE;

    /*
     * seamless migration support was added to the client after the support in
     * agent_connect_tokens, so there shouldn't be contradicition - if
     * the client is capable of seamless migration, it is capbable of agent_connected_tokens.
     * The demand for agent_connected_tokens support is in order to assure that if migration
     * occured when the agent was not connected, the tokens state after migration will still
     * be valid (see reds_reset_vdp for more details).
     */
    try_seamless = reds->seamless_migration_enabled &&
                   red_channel_test_remote_cap(&reds->main_channel->base,
                   SPICE_MAIN_CAP_AGENT_CONNECTED_TOKENS);
    /* main channel will take care of clients that are still during migration (at target)*/
    if (main_channel_migrate_connect(reds->main_channel, reds->mig_spice,
                                     try_seamless)) {
        reds_mig_started();
    } else {
        if (reds->num_clients == 0) {
            reds_mig_release();
            spice_info("no client connected");
        }
        sif->migrate_connect_complete(migration_interface);
    }

    return 0;
}
