static void reds_mig_disconnect(void)
{
    if (reds_main_channel_connected()) {
        reds_disconnect();
    } else {
        reds_mig_cleanup();
    }
}
