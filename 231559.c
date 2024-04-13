void ftrace_exports(struct ring_buffer_event *event)
{
	struct trace_export *export;

	preempt_disable_notrace();

	export = rcu_dereference_raw_notrace(ftrace_exports_list);
	while (export) {
		trace_process_export(export, event);
		export = rcu_dereference_raw_notrace(export->next);
	}

	preempt_enable_notrace();
}