static void reds_mig_cleanup(void)
{
    if (reds->mig_inprogress) {

        if (reds->mig_wait_connect || reds->mig_wait_disconnect) {
            SpiceMigrateInterface *sif;
            spice_assert(migration_interface);
            sif = SPICE_CONTAINEROF(migration_interface->base.sif, SpiceMigrateInterface, base);
            if (reds->mig_wait_connect) {
                sif->migrate_connect_complete(migration_interface);
            } else {
                if (sif->migrate_end_complete) {
                    sif->migrate_end_complete(migration_interface);
                }
            }
        }
        reds->mig_inprogress = FALSE;
        reds->mig_wait_connect = FALSE;
        reds->mig_wait_disconnect = FALSE;
        core->timer_cancel(reds->mig_timer);
        reds_mig_cleanup_wait_disconnect();
    }
}
