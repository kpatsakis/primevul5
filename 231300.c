static int trace_save_cmdline(struct task_struct *tsk)
{
	unsigned pid, idx;

	/* treat recording of idle task as a success */
	if (!tsk->pid)
		return 1;

	if (unlikely(tsk->pid > PID_MAX_DEFAULT))
		return 0;

	/*
	 * It's not the end of the world if we don't get
	 * the lock, but we also don't want to spin
	 * nor do we want to disable interrupts,
	 * so if we miss here, then better luck next time.
	 */
	if (!arch_spin_trylock(&trace_cmdline_lock))
		return 0;

	idx = savedcmd->map_pid_to_cmdline[tsk->pid];
	if (idx == NO_CMDLINE_MAP) {
		idx = (savedcmd->cmdline_idx + 1) % savedcmd->cmdline_num;

		/*
		 * Check whether the cmdline buffer at idx has a pid
		 * mapped. We are going to overwrite that entry so we
		 * need to clear the map_pid_to_cmdline. Otherwise we
		 * would read the new comm for the old pid.
		 */
		pid = savedcmd->map_cmdline_to_pid[idx];
		if (pid != NO_CMDLINE_MAP)
			savedcmd->map_pid_to_cmdline[pid] = NO_CMDLINE_MAP;

		savedcmd->map_cmdline_to_pid[idx] = tsk->pid;
		savedcmd->map_pid_to_cmdline[tsk->pid] = idx;

		savedcmd->cmdline_idx = idx;
	}

	set_cmdline(idx, tsk->comm);

	arch_spin_unlock(&trace_cmdline_lock);

	return 1;
}