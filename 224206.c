static bool recalc_sigpending_tsk(struct task_struct *t)
{
	if ((t->jobctl & (JOBCTL_PENDING_MASK | JOBCTL_TRAP_FREEZE)) ||
	    PENDING(&t->pending, &t->blocked) ||
	    PENDING(&t->signal->shared_pending, &t->blocked) ||
	    cgroup_task_frozen(t)) {
		set_tsk_thread_flag(t, TIF_SIGPENDING);
		return true;
	}

	/*
	 * We must never clear the flag in another thread, or in current
	 * when it's possible the current syscall is returning -ERESTART*.
	 * So we don't clear it here, and only callers who know they should do.
	 */
	return false;
}