	__releases(&current->sighand->siglock)
{
	/*
	 * If there are other trap bits pending except JOBCTL_TRAP_FREEZE,
	 * let's make another loop to give it a chance to be handled.
	 * In any case, we'll return back.
	 */
	if ((current->jobctl & (JOBCTL_PENDING_MASK | JOBCTL_TRAP_FREEZE)) !=
	     JOBCTL_TRAP_FREEZE) {
		spin_unlock_irq(&current->sighand->siglock);
		return;
	}

	/*
	 * Now we're sure that there is no pending fatal signal and no
	 * pending traps. Clear TIF_SIGPENDING to not get out of schedule()
	 * immediately (if there is a non-fatal signal pending), and
	 * put the task into sleep.
	 */
	__set_current_state(TASK_INTERRUPTIBLE);
	clear_thread_flag(TIF_SIGPENDING);
	spin_unlock_irq(&current->sighand->siglock);
	cgroup_enter_frozen();
	freezable_schedule();
}