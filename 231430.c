static int run_tracer_selftest(struct tracer *type)
{
	struct trace_array *tr = &global_trace;
	struct tracer *saved_tracer = tr->current_trace;
	int ret;

	if (!type->selftest || tracing_selftest_disabled)
		return 0;

	/*
	 * If a tracer registers early in boot up (before scheduling is
	 * initialized and such), then do not run its selftests yet.
	 * Instead, run it a little later in the boot process.
	 */
	if (!selftests_can_run)
		return save_selftest(type);

	/*
	 * Run a selftest on this tracer.
	 * Here we reset the trace buffer, and set the current
	 * tracer to be this tracer. The tracer can then run some
	 * internal tracing to verify that everything is in order.
	 * If we fail, we do not register this tracer.
	 */
	tracing_reset_online_cpus(&tr->trace_buffer);

	tr->current_trace = type;

#ifdef CONFIG_TRACER_MAX_TRACE
	if (type->use_max_tr) {
		/* If we expanded the buffers, make sure the max is expanded too */
		if (ring_buffer_expanded)
			ring_buffer_resize(tr->max_buffer.buffer, trace_buf_size,
					   RING_BUFFER_ALL_CPUS);
		tr->allocated_snapshot = true;
	}
#endif

	/* the test is responsible for initializing and enabling */
	pr_info("Testing tracer %s: ", type->name);
	ret = type->selftest(type, tr);
	/* the test is responsible for resetting too */
	tr->current_trace = saved_tracer;
	if (ret) {
		printk(KERN_CONT "FAILED!\n");
		/* Add the warning after printing 'FAILED' */
		WARN_ON(1);
		return -1;
	}
	/* Only reset on passing, to avoid touching corrupted buffers */
	tracing_reset_online_cpus(&tr->trace_buffer);

#ifdef CONFIG_TRACER_MAX_TRACE
	if (type->use_max_tr) {
		tr->allocated_snapshot = false;

		/* Shrink the max buffer again */
		if (ring_buffer_expanded)
			ring_buffer_resize(tr->max_buffer.buffer, 1,
					   RING_BUFFER_ALL_CPUS);
	}
#endif

	printk(KERN_CONT "PASSED\n");
	return 0;
}