static void __init trace_eval_init(void)
{
	int len;

	len = __stop_ftrace_eval_maps - __start_ftrace_eval_maps;
	trace_insert_eval_map(NULL, __start_ftrace_eval_maps, len);
}