void trace_free_pid_list(struct trace_pid_list *pid_list)
{
	vfree(pid_list->pids);
	kfree(pid_list);
}