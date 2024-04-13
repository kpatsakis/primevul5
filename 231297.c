void *trace_pid_next(struct trace_pid_list *pid_list, void *v, loff_t *pos)
{
	unsigned long pid = (unsigned long)v;

	(*pos)++;

	/* pid already is +1 of the actual prevous bit */
	pid = find_next_bit(pid_list->pids, pid_list->pid_max, pid);

	/* Return pid + 1 to allow zero to be represented */
	if (pid < pid_list->pid_max)
		return (void *)(pid + 1);

	return NULL;
}