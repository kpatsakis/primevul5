void reds_disable_mm_timer(void)
{
    core->timer_cancel(reds->mm_timer);
    reds->mm_timer_enabled = FALSE;
}
