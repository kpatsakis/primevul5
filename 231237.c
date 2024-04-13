static void print_func_help_header_irq(struct trace_buffer *buf, struct seq_file *m,
				       unsigned int flags)
{
	bool tgid = flags & TRACE_ITER_RECORD_TGID;
	const char tgid_space[] = "          ";
	const char space[] = "  ";

	seq_printf(m, "#                          %s  _-----=> irqs-off\n",
		   tgid ? tgid_space : space);
	seq_printf(m, "#                          %s / _----=> need-resched\n",
		   tgid ? tgid_space : space);
	seq_printf(m, "#                          %s| / _---=> hardirq/softirq\n",
		   tgid ? tgid_space : space);
	seq_printf(m, "#                          %s|| / _--=> preempt-depth\n",
		   tgid ? tgid_space : space);
	seq_printf(m, "#                          %s||| /     delay\n",
		   tgid ? tgid_space : space);
	seq_printf(m, "#           TASK-PID   CPU#%s||||    TIMESTAMP  FUNCTION\n",
		   tgid ? "   TGID   " : space);
	seq_printf(m, "#              | |       | %s||||       |         |\n",
		   tgid ? "     |    " : space);
}