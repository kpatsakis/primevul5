int flush_old_exec(struct linux_binprm * bprm)
{
	struct task_struct *me = current;
	int retval;

	/*
	 * Make this the only thread in the thread group.
	 */
	retval = de_thread(me);
	if (retval)
		goto out;

	/*
	 * Must be called _before_ exec_mmap() as bprm->mm is
	 * not visibile until then. This also enables the update
	 * to be lockless.
	 */
	set_mm_exe_file(bprm->mm, bprm->file);

	/*
	 * Release all of the old mmap stuff
	 */
	acct_arg_size(bprm, 0);
	retval = exec_mmap(bprm->mm);
	if (retval)
		goto out;

	/*
	 * After setting bprm->called_exec_mmap (to mark that current is
	 * using the prepared mm now), we have nothing left of the original
	 * process. If anything from here on returns an error, the check
	 * in search_binary_handler() will SEGV current.
	 */
	bprm->called_exec_mmap = 1;
	bprm->mm = NULL;

#ifdef CONFIG_POSIX_TIMERS
	exit_itimers(me->signal);
	flush_itimer_signals();
#endif

	/*
	 * Make the signal table private.
	 */
	retval = unshare_sighand(me);
	if (retval)
		goto out;

	set_fs(USER_DS);
	me->flags &= ~(PF_RANDOMIZE | PF_FORKNOEXEC | PF_KTHREAD |
					PF_NOFREEZE | PF_NO_SETAFFINITY);
	flush_thread();
	me->personality &= ~bprm->per_clear;

	/*
	 * We have to apply CLOEXEC before we change whether the process is
	 * dumpable (in setup_new_exec) to avoid a race with a process in userspace
	 * trying to access the should-be-closed file descriptors of a process
	 * undergoing exec(2).
	 */
	do_close_on_exec(me->files);
	return 0;

out:
	return retval;
}