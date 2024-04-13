static void migrate_timeout(void *opaque)
{
    spice_info(NULL);
    spice_assert(reds->mig_wait_connect || reds->mig_wait_disconnect);
    if (reds->mig_wait_connect) {
        /* we will fall back to the switch host scheme when migration completes */
        main_channel_migrate_cancel_wait(reds->main_channel);
        /* in case part of the client haven't yet completed the previous migration, disconnect them */
        reds_mig_target_client_disconnect_all();
        reds_mig_cleanup();
    } else {
        reds_mig_disconnect();
    }
}
