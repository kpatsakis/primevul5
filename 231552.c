void trace_filter_add_remove_task(struct trace_pid_list *pid_list,
				  struct task_struct *self,
				  struct task_struct *task)
{
	if (!pid_list)
		return;

	/* For forks, we only add if the forking task is listed */
	if (self) {
		if (!trace_find_filtered_pid(pid_list, self->pid))
			return;
	}

	/* Sorry, but we don't support pid_max changing after setting */
	if (task->pid >= pid_list->pid_max)
		return;

	/* "self" is set for forks, and NULL for exits */
	if (self)
		set_bit(task->pid, pid_list->pids);
	else
		clear_bit(task->pid, pid_list->pids);
}