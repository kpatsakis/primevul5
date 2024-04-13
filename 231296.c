void trace_buffered_event_enable(void)
{
	struct ring_buffer_event *event;
	struct page *page;
	int cpu;

	WARN_ON_ONCE(!mutex_is_locked(&event_mutex));

	if (trace_buffered_event_ref++)
		return;

	for_each_tracing_cpu(cpu) {
		page = alloc_pages_node(cpu_to_node(cpu),
					GFP_KERNEL | __GFP_NORETRY, 0);
		if (!page)
			goto failed;

		event = page_address(page);
		memset(event, 0, sizeof(*event));

		per_cpu(trace_buffered_event, cpu) = event;

		preempt_disable();
		if (cpu == smp_processor_id() &&
		    this_cpu_read(trace_buffered_event) !=
		    per_cpu(trace_buffered_event, cpu))
			WARN_ON_ONCE(1);
		preempt_enable();
	}

	return;
 failed:
	trace_buffered_event_disable();
}