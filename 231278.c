static int eval_map_show(struct seq_file *m, void *v)
{
	union trace_eval_map_item *ptr = v;

	seq_printf(m, "%s %ld (%s)\n",
		   ptr->map.eval_string, ptr->map.eval_value,
		   ptr->map.system);

	return 0;
}