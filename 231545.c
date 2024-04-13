static int instance_mkdir(const char *name)
{
	struct trace_array *tr;
	int ret;

	mutex_lock(&event_mutex);
	mutex_lock(&trace_types_lock);

	ret = -EEXIST;
	list_for_each_entry(tr, &ftrace_trace_arrays, list) {
		if (tr->name && strcmp(tr->name, name) == 0)
			goto out_unlock;
	}

	ret = -ENOMEM;
	tr = kzalloc(sizeof(*tr), GFP_KERNEL);
	if (!tr)
		goto out_unlock;

	tr->name = kstrdup(name, GFP_KERNEL);
	if (!tr->name)
		goto out_free_tr;

	if (!alloc_cpumask_var(&tr->tracing_cpumask, GFP_KERNEL))
		goto out_free_tr;

	tr->trace_flags = global_trace.trace_flags & ~ZEROED_TRACE_FLAGS;

	cpumask_copy(tr->tracing_cpumask, cpu_all_mask);

	raw_spin_lock_init(&tr->start_lock);

	tr->max_lock = (arch_spinlock_t)__ARCH_SPIN_LOCK_UNLOCKED;

	tr->current_trace = &nop_trace;

	INIT_LIST_HEAD(&tr->systems);
	INIT_LIST_HEAD(&tr->events);

	if (allocate_trace_buffers(tr, trace_buf_size) < 0)
		goto out_free_tr;

	tr->dir = tracefs_create_dir(name, trace_instance_dir);
	if (!tr->dir)
		goto out_free_tr;

	ret = event_trace_add_tracer(tr->dir, tr);
	if (ret) {
		tracefs_remove_recursive(tr->dir);
		goto out_free_tr;
	}

	ftrace_init_trace_array(tr);

	init_tracer_tracefs(tr, tr->dir);
	init_trace_flags_index(tr);
	__update_tracer_options(tr);

	list_add(&tr->list, &ftrace_trace_arrays);

	mutex_unlock(&trace_types_lock);
	mutex_unlock(&event_mutex);

	return 0;

 out_free_tr:
	free_trace_buffers(tr);
	free_cpumask_var(tr->tracing_cpumask);
	kfree(tr->name);
	kfree(tr);

 out_unlock:
	mutex_unlock(&trace_types_lock);
	mutex_unlock(&event_mutex);

	return ret;

}