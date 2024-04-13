static void *s_start(struct seq_file *m, loff_t *pos)
{
	struct trace_iterator *iter = m->private;
	struct trace_array *tr = iter->tr;
	int cpu_file = iter->cpu_file;
	void *p = NULL;
	loff_t l = 0;
	int cpu;

	/*
	 * copy the tracer to avoid using a global lock all around.
	 * iter->trace is a copy of current_trace, the pointer to the
	 * name may be used instead of a strcmp(), as iter->trace->name
	 * will point to the same string as current_trace->name.
	 */
	mutex_lock(&trace_types_lock);
	if (unlikely(tr->current_trace && iter->trace->name != tr->current_trace->name))
		*iter->trace = *tr->current_trace;
	mutex_unlock(&trace_types_lock);

#ifdef CONFIG_TRACER_MAX_TRACE
	if (iter->snapshot && iter->trace->use_max_tr)
		return ERR_PTR(-EBUSY);
#endif

	if (!iter->snapshot)
		atomic_inc(&trace_record_taskinfo_disabled);

	if (*pos != iter->pos) {
		iter->ent = NULL;
		iter->cpu = 0;
		iter->idx = -1;

		if (cpu_file == RING_BUFFER_ALL_CPUS) {
			for_each_tracing_cpu(cpu)
				tracing_iter_reset(iter, cpu);
		} else
			tracing_iter_reset(iter, cpu_file);

		iter->leftover = 0;
		for (p = iter; p && l < *pos; p = s_next(m, p, &l))
			;

	} else {
		/*
		 * If we overflowed the seq_file before, then we want
		 * to just reuse the trace_seq buffer again.
		 */
		if (iter->leftover)
			p = iter;
		else {
			l = *pos - 1;
			p = s_next(m, p, &l);
		}
	}

	trace_event_read_lock();
	trace_access_lock(cpu_file);
	return p;
}