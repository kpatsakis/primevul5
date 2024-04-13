static int do_sigtimedwait(const sigset_t *which, kernel_siginfo_t *info,
		    const struct timespec64 *ts)
{
	ktime_t *to = NULL, timeout = KTIME_MAX;
	struct task_struct *tsk = current;
	sigset_t mask = *which;
	int sig, ret = 0;

	if (ts) {
		if (!timespec64_valid(ts))
			return -EINVAL;
		timeout = timespec64_to_ktime(*ts);
		to = &timeout;
	}

	/*
	 * Invert the set of allowed signals to get those we want to block.
	 */
	sigdelsetmask(&mask, sigmask(SIGKILL) | sigmask(SIGSTOP));
	signotset(&mask);

	spin_lock_irq(&tsk->sighand->siglock);
	sig = dequeue_signal(tsk, &mask, info);
	if (!sig && timeout) {
		/*
		 * None ready, temporarily unblock those we're interested
		 * while we are sleeping in so that we'll be awakened when
		 * they arrive. Unblocking is always fine, we can avoid
		 * set_current_blocked().
		 */
		tsk->real_blocked = tsk->blocked;
		sigandsets(&tsk->blocked, &tsk->blocked, &mask);
		recalc_sigpending();
		spin_unlock_irq(&tsk->sighand->siglock);

		__set_current_state(TASK_INTERRUPTIBLE);
		ret = freezable_schedule_hrtimeout_range(to, tsk->timer_slack_ns,
							 HRTIMER_MODE_REL);
		spin_lock_irq(&tsk->sighand->siglock);
		__set_task_blocked(tsk, &tsk->real_blocked);
		sigemptyset(&tsk->real_blocked);
		sig = dequeue_signal(tsk, &mask, info);
	}
	spin_unlock_irq(&tsk->sighand->siglock);

	if (sig)
		return sig;
	return ret ? -EINTR : -EAGAIN;
}