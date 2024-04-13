static int instance_rmdir(const char *name)
{
	struct trace_array *tr;
	int found = 0;
	int ret;
	int i;

	mutex_lock(&event_mutex);
	mutex_lock(&trace_types_lock);

	ret = -ENODEV;
	list_for_each_entry(tr, &ftrace_trace_arrays, list) {
		if (tr->name && strcmp(tr->name, name) == 0) {
			found = 1;
			break;
		}
	}
	if (!found)
		goto out_unlock;

	ret = -EBUSY;
	if (tr->ref || (tr->current_trace && tr->current_trace->ref))
		goto out_unlock;

	list_del(&tr->list);

	/* Disable all the flags that were enabled coming in */
	for (i = 0; i < TRACE_FLAGS_MAX_SIZE; i++) {
		if ((1 << i) & ZEROED_TRACE_FLAGS)
			set_tracer_flag(tr, 1 << i, 0);
	}

	tracing_set_nop(tr);
	clear_ftrace_function_probes(tr);
	event_trace_del_tracer(tr);
	ftrace_clear_pids(tr);
	ftrace_destroy_function_files(tr);
	tracefs_remove_recursive(tr->dir);
	free_trace_buffers(tr);

	for (i = 0; i < tr->nr_topts; i++) {
		kfree(tr->topts[i].topts);
	}
	kfree(tr->topts);

	free_cpumask_var(tr->tracing_cpumask);
	kfree(tr->name);
	kfree(tr);

	ret = 0;

 out_unlock:
	mutex_unlock(&trace_types_lock);
	mutex_unlock(&event_mutex);

	return ret;
}