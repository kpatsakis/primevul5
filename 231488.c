t_next(struct seq_file *m, void *v, loff_t *pos)
{
	struct trace_array *tr = m->private;
	struct tracer *t = v;

	(*pos)++;

	if (t)
		t = get_tracer_for_array(tr, t->next);

	return t;
}