static void reds_mig_switch(void)
{
    if (!reds->mig_spice) {
        spice_warning("reds_mig_switch called without migrate_info set");
        return;
    }
    main_channel_migrate_switch(reds->main_channel, reds->mig_spice);
    reds_mig_release();
}
