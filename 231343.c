static void *eval_map_start(struct seq_file *m, loff_t *pos)
{
	union trace_eval_map_item *v;
	loff_t l = 0;

	mutex_lock(&trace_eval_mutex);

	v = trace_eval_maps;
	if (v)
		v++;

	while (v && l < *pos) {
		v = eval_map_next(m, v, &l);
	}

	return v;
}