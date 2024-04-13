static void trace_module_remove_evals(struct module *mod)
{
	union trace_eval_map_item *map;
	union trace_eval_map_item **last = &trace_eval_maps;

	if (!mod->num_trace_evals)
		return;

	mutex_lock(&trace_eval_mutex);

	map = trace_eval_maps;

	while (map) {
		if (map->head.mod == mod)
			break;
		map = trace_eval_jmp_to_tail(map);
		last = &map->tail.next;
		map = map->tail.next;
	}
	if (!map)
		goto out;

	*last = trace_eval_jmp_to_tail(map)->tail.next;
	kfree(map);
 out:
	mutex_unlock(&trace_eval_mutex);
}