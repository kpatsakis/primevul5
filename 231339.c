static int allocate_cmdlines_buffer(unsigned int val,
				    struct saved_cmdlines_buffer *s)
{
	s->map_cmdline_to_pid = kmalloc(val * sizeof(*s->map_cmdline_to_pid),
					GFP_KERNEL);
	if (!s->map_cmdline_to_pid)
		return -ENOMEM;

	s->saved_cmdlines = kmalloc(val * TASK_COMM_LEN, GFP_KERNEL);
	if (!s->saved_cmdlines) {
		kfree(s->map_cmdline_to_pid);
		return -ENOMEM;
	}

	s->cmdline_idx = 0;
	s->cmdline_num = val;
	memset(&s->map_pid_to_cmdline, NO_CMDLINE_MAP,
	       sizeof(s->map_pid_to_cmdline));
	memset(s->map_cmdline_to_pid, NO_CMDLINE_MAP,
	       val * sizeof(*s->map_cmdline_to_pid));

	return 0;
}