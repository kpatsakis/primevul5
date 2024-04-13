static void print_func_help_header(struct trace_buffer *buf, struct seq_file *m,
				   unsigned int flags)
{
	bool tgid = flags & TRACE_ITER_RECORD_TGID;

	print_event_info(buf, m);

	seq_printf(m, "#           TASK-PID   CPU#   %s  TIMESTAMP  FUNCTION\n", tgid ? "TGID     " : "");
	seq_printf(m, "#              | |       |    %s     |         |\n",	 tgid ? "  |      " : "");
}