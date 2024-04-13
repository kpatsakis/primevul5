static void s_stop(struct seq_file *m, void *p)
{
	struct trace_iterator *iter = m->private;

#ifdef CONFIG_TRACER_MAX_TRACE
	if (iter->snapshot && iter->trace->use_max_tr)
		return;
#endif

	if (!iter->snapshot)
		atomic_dec(&trace_record_taskinfo_disabled);

	trace_access_unlock(iter->cpu_file);
	trace_event_read_unlock();
}