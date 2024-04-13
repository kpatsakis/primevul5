int trace_pid_write(struct trace_pid_list *filtered_pids,
		    struct trace_pid_list **new_pid_list,
		    const char __user *ubuf, size_t cnt)
{
	struct trace_pid_list *pid_list;
	struct trace_parser parser;
	unsigned long val;
	int nr_pids = 0;
	ssize_t read = 0;
	ssize_t ret = 0;
	loff_t pos;
	pid_t pid;

	if (trace_parser_get_init(&parser, PID_BUF_SIZE + 1))
		return -ENOMEM;

	/*
	 * Always recreate a new array. The write is an all or nothing
	 * operation. Always create a new array when adding new pids by
	 * the user. If the operation fails, then the current list is
	 * not modified.
	 */
	pid_list = kmalloc(sizeof(*pid_list), GFP_KERNEL);
	if (!pid_list)
		return -ENOMEM;

	pid_list->pid_max = READ_ONCE(pid_max);

	/* Only truncating will shrink pid_max */
	if (filtered_pids && filtered_pids->pid_max > pid_list->pid_max)
		pid_list->pid_max = filtered_pids->pid_max;

	pid_list->pids = vzalloc((pid_list->pid_max + 7) >> 3);
	if (!pid_list->pids) {
		kfree(pid_list);
		return -ENOMEM;
	}

	if (filtered_pids) {
		/* copy the current bits to the new max */
		for_each_set_bit(pid, filtered_pids->pids,
				 filtered_pids->pid_max) {
			set_bit(pid, pid_list->pids);
			nr_pids++;
		}
	}

	while (cnt > 0) {

		pos = 0;

		ret = trace_get_user(&parser, ubuf, cnt, &pos);
		if (ret < 0 || !trace_parser_loaded(&parser))
			break;

		read += ret;
		ubuf += ret;
		cnt -= ret;

		parser.buffer[parser.idx] = 0;

		ret = -EINVAL;
		if (kstrtoul(parser.buffer, 0, &val))
			break;
		if (val >= pid_list->pid_max)
			break;

		pid = (pid_t)val;

		set_bit(pid, pid_list->pids);
		nr_pids++;

		trace_parser_clear(&parser);
		ret = 0;
	}
	trace_parser_put(&parser);

	if (ret < 0) {
		trace_free_pid_list(pid_list);
		return ret;
	}

	if (!nr_pids) {
		/* Cleared the list of pids */
		trace_free_pid_list(pid_list);
		read = ret;
		pid_list = NULL;
	}

	*new_pid_list = pid_list;

	return read;
}