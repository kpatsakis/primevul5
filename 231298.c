static void *t_start(struct seq_file *m, loff_t *pos)
{
	struct trace_array *tr = m->private;
	struct tracer *t;
	loff_t l = 0;

	mutex_lock(&trace_types_lock);

	t = get_tracer_for_array(tr, trace_types);
	for (; t && l < *pos; t = t_next(m, t, &l))
			;

	return t;
}