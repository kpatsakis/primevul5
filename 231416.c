static int trace_save_tgid(struct task_struct *tsk)
{
	/* treat recording of idle task as a success */
	if (!tsk->pid)
		return 1;

	if (unlikely(!tgid_map || tsk->pid > PID_MAX_DEFAULT))
		return 0;

	tgid_map[tsk->pid] = tsk->tgid;
	return 1;
}