void tracing_record_tgid(struct task_struct *task)
{
	tracing_record_taskinfo(task, TRACE_RECORD_TGID);
}