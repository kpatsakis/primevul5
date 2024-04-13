static void *saved_cmdlines_start(struct seq_file *m, loff_t *pos)
{
	void *v;
	loff_t l = 0;

	preempt_disable();
	arch_spin_lock(&trace_cmdline_lock);

	v = &savedcmd->map_cmdline_to_pid[0];
	while (l <= *pos) {
		v = saved_cmdlines_next(m, v, &l);
		if (!v)
			return NULL;
	}

	return v;
}