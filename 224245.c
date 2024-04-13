void calculate_sigpending(void)
{
	/* Have any signals or users of TIF_SIGPENDING been delayed
	 * until after fork?
	 */
	spin_lock_irq(&current->sighand->siglock);
	set_tsk_thread_flag(current, TIF_SIGPENDING);
	recalc_sigpending();
	spin_unlock_irq(&current->sighand->siglock);
}