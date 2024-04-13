void *trace_pid_start(struct trace_pid_list *pid_list, loff_t *pos)
{
	unsigned long pid;
	loff_t l = 0;

	pid = find_first_bit(pid_list->pids, pid_list->pid_max);
	if (pid >= pid_list->pid_max)
		return NULL;

	/* Return pid + 1 so that zero can be the exit value */
	for (pid++; pid && l < *pos;
	     pid = (unsigned long)trace_pid_next(pid_list, (void *)pid, &l))
		;
	return (void *)pid;
}