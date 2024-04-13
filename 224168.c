void recalc_sigpending(void)
{
	if (!recalc_sigpending_tsk(current) && !freezing(current) &&
	    !klp_patch_pending(current))
		clear_thread_flag(TIF_SIGPENDING);

}