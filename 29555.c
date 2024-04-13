static void reds_send_mm_time(void)
{
    if (!reds_main_channel_connected()) {
        return;
    }
    spice_debug(NULL);
    main_channel_push_multi_media_time(reds->main_channel,
                                       reds_get_mm_time() - reds->mm_time_latency);
}
