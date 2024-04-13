trace_find_filtered_pid(struct trace_pid_list *filtered_pids, pid_t search_pid)
{
	/*
	 * If pid_max changed after filtered_pids was created, we
	 * by default ignore all pids greater than the previous pid_max.
	 */
	if (search_pid >= filtered_pids->pid_max)
		return false;

	return test_bit(search_pid, filtered_pids->pids);
}