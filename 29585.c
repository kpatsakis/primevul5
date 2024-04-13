SPICE_GNUC_VISIBLE int spice_server_migrate_end(SpiceServer *s, int completed)
{
    SpiceMigrateInterface *sif;
    int ret = 0;

    spice_info(NULL);

    spice_assert(migration_interface);
    spice_assert(reds == s);

    sif = SPICE_CONTAINEROF(migration_interface->base.sif, SpiceMigrateInterface, base);
    if (completed && !reds->expect_migrate && reds->num_clients) {
        spice_warning("spice_server_migrate_info was not called, disconnecting clients");
        reds_disconnect();
        ret = -1;
        goto complete;
    }

    reds->expect_migrate = FALSE;
    if (!reds_main_channel_connected()) {
        spice_info("no peer connected");
        goto complete;
    }
    reds_mig_finished(completed);
    return 0;
complete:
    if (sif->migrate_end_complete) {
        sif->migrate_end_complete(migration_interface);
    }
    return ret;
}
