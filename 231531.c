static void print_snapshot_help(struct seq_file *m, struct trace_iterator *iter)
{
	if (iter->tr->allocated_snapshot)
		seq_puts(m, "#\n# * Snapshot is allocated *\n#\n");
	else
		seq_puts(m, "#\n# * Snapshot is freed *\n#\n");

	seq_puts(m, "# Snapshot commands:\n");
	if (iter->cpu_file == RING_BUFFER_ALL_CPUS)
		show_snapshot_main_help(m);
	else
		show_snapshot_percpu_help(m);
}