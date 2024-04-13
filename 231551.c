static void trace_insert_eval_map(struct module *mod,
				  struct trace_eval_map **start, int len)
{
	struct trace_eval_map **map;

	if (len <= 0)
		return;

	map = start;

	trace_event_eval_update(map, len);

	trace_insert_eval_map_file(mod, start, len);
}