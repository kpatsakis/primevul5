void tracing_record_cmdline(struct task_struct *task)
{
	tracing_record_taskinfo(task, TRACE_RECORD_CMDLINE);
}