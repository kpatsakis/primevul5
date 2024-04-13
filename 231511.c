int tracer_init(struct tracer *t, struct trace_array *tr)
{
	tracing_reset_online_cpus(&tr->trace_buffer);
	return t->init(tr);
}