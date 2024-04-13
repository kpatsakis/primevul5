void reds_set_client_mm_time_latency(RedClient *client, uint32_t latency)
{
    if (reds->mm_timer_enabled) {
        if (latency > reds->mm_time_latency) {
            reds->mm_time_latency = latency;
            reds_send_mm_time();
        } else {
            spice_debug("new latency %u is smaller than existing %u",
                        latency, reds->mm_time_latency);
        }
    } else {
        snd_set_playback_latency(client, latency);
    }
}
