static int trace_panic_handler(struct notifier_block *this,
			       unsigned long event, void *unused)
{
	if (ftrace_dump_on_oops)
		ftrace_dump(ftrace_dump_on_oops);
	return NOTIFY_OK;
}