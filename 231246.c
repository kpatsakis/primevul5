trace_ignore_this_task(struct trace_pid_list *filtered_pids, struct task_struct *task)
{
	/*
	 * Return false, because if filtered_pids does not exist,
	 * all pids are good to trace.
	 */
	if (!filtered_pids)
		return false;

	return !trace_find_filtered_pid(filtered_pids, task->pid);
}