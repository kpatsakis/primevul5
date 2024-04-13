static void tracing_snapshot_instance(struct trace_array *tr)
{
	struct tracer *tracer = tr->current_trace;
	unsigned long flags;

	if (in_nmi()) {
		internal_trace_puts("*** SNAPSHOT CALLED FROM NMI CONTEXT ***\n");
		internal_trace_puts("*** snapshot is being ignored        ***\n");
		return;
	}

	if (!tr->allocated_snapshot) {
		internal_trace_puts("*** SNAPSHOT NOT ALLOCATED ***\n");
		internal_trace_puts("*** stopping trace here!   ***\n");
		tracing_off();
		return;
	}

	/* Note, snapshot can not be used when the tracer uses it */
	if (tracer->use_max_tr) {
		internal_trace_puts("*** LATENCY TRACER ACTIVE ***\n");
		internal_trace_puts("*** Can not use snapshot (sorry) ***\n");
		return;
	}

	local_irq_save(flags);
	update_max_tr(tr, current, smp_processor_id());
	local_irq_restore(flags);
}