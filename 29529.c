static void reds_mig_finished(int completed)
{
    spice_info(NULL);

    reds->mig_inprogress = TRUE;

    if (reds->src_do_seamless_migrate && completed) {
        reds_migrate_channels_seamless();
    } else {
        main_channel_migrate_src_complete(reds->main_channel, completed);
    }

    if (completed) {
        reds_mig_fill_wait_disconnect();
    } else {
        reds_mig_cleanup();
    }
    reds_mig_release();
}
