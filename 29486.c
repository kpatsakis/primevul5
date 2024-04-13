void reds_enable_mm_timer(void)
{
    core->timer_start(reds->mm_timer, MM_TIMER_GRANULARITY_MS);
    reds->mm_timer_enabled = TRUE;
    reds->mm_time_latency = MM_TIME_DELTA;
    reds_send_mm_time();
}
