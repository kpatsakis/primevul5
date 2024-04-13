ftrace_snapshot_print(struct seq_file *m, unsigned long ip,
		      struct ftrace_probe_ops *ops, void *data)
{
	struct ftrace_func_mapper *mapper = data;
	long *count = NULL;

	seq_printf(m, "%ps:", (void *)ip);

	seq_puts(m, "snapshot");

	if (mapper)
		count = (long *)ftrace_func_mapper_find_ip(mapper, ip);

	if (count)
		seq_printf(m, ":count=%ld\n", *count);
	else
		seq_puts(m, ":unlimited\n");

	return 0;
}