__update_max_tr(struct trace_array *tr, struct task_struct *tsk, int cpu)
{
	struct trace_buffer *trace_buf = &tr->trace_buffer;
	struct trace_buffer *max_buf = &tr->max_buffer;
	struct trace_array_cpu *data = per_cpu_ptr(trace_buf->data, cpu);
	struct trace_array_cpu *max_data = per_cpu_ptr(max_buf->data, cpu);

	max_buf->cpu = cpu;
	max_buf->time_start = data->preempt_timestamp;

	max_data->saved_latency = tr->max_latency;
	max_data->critical_start = data->critical_start;
	max_data->critical_end = data->critical_end;

	memcpy(max_data->comm, tsk->comm, TASK_COMM_LEN);
	max_data->pid = tsk->pid;
	/*
	 * If tsk == current, then use current_uid(), as that does not use
	 * RCU. The irq tracer can be called out of RCU scope.
	 */
	if (tsk == current)
		max_data->uid = current_uid();
	else
		max_data->uid = task_uid(tsk);

	max_data->nice = tsk->static_prio - 20 - MAX_RT_PRIO;
	max_data->policy = tsk->policy;
	max_data->rt_priority = tsk->rt_priority;

	/* record this tasks comm */
	tracing_record_cmdline(tsk);
}