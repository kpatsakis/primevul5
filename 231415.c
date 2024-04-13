static int tracing_set_tracer(struct trace_array *tr, const char *buf)
{
	struct tracer *t;
#ifdef CONFIG_TRACER_MAX_TRACE
	bool had_max_tr;
#endif
	int ret = 0;

	mutex_lock(&trace_types_lock);

	if (!ring_buffer_expanded) {
		ret = __tracing_resize_ring_buffer(tr, trace_buf_size,
						RING_BUFFER_ALL_CPUS);
		if (ret < 0)
			goto out;
		ret = 0;
	}

	for (t = trace_types; t; t = t->next) {
		if (strcmp(t->name, buf) == 0)
			break;
	}
	if (!t) {
		ret = -EINVAL;
		goto out;
	}
	if (t == tr->current_trace)
		goto out;

	/* Some tracers won't work on kernel command line */
	if (system_state < SYSTEM_RUNNING && t->noboot) {
		pr_warn("Tracer '%s' is not allowed on command line, ignored\n",
			t->name);
		goto out;
	}

	/* Some tracers are only allowed for the top level buffer */
	if (!trace_ok_for_array(t, tr)) {
		ret = -EINVAL;
		goto out;
	}

	/* If trace pipe files are being read, we can't change the tracer */
	if (tr->current_trace->ref) {
		ret = -EBUSY;
		goto out;
	}

	trace_branch_disable();

	tr->current_trace->enabled--;

	if (tr->current_trace->reset)
		tr->current_trace->reset(tr);

	/* Current trace needs to be nop_trace before synchronize_sched */
	tr->current_trace = &nop_trace;

#ifdef CONFIG_TRACER_MAX_TRACE
	had_max_tr = tr->allocated_snapshot;

	if (had_max_tr && !t->use_max_tr) {
		/*
		 * We need to make sure that the update_max_tr sees that
		 * current_trace changed to nop_trace to keep it from
		 * swapping the buffers after we resize it.
		 * The update_max_tr is called from interrupts disabled
		 * so a synchronized_sched() is sufficient.
		 */
		synchronize_sched();
		free_snapshot(tr);
	}
#endif

#ifdef CONFIG_TRACER_MAX_TRACE
	if (t->use_max_tr && !had_max_tr) {
		ret = alloc_snapshot(tr);
		if (ret < 0)
			goto out;
	}
#endif

	if (t->init) {
		ret = tracer_init(t, tr);
		if (ret)
			goto out;
	}

	tr->current_trace = t;
	tr->current_trace->enabled++;
	trace_branch_enable(tr);
 out:
	mutex_unlock(&trace_types_lock);

	return ret;
}