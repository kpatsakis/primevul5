static void test_ftrace_alive(struct seq_file *m)
{
	if (!ftrace_is_dead())
		return;
	seq_puts(m, "# WARNING: FUNCTION TRACING IS CORRUPTED\n"
		    "#          MAY BE MISSING FUNCTION EVENTS\n");
}