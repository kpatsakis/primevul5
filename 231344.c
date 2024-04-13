static int set_tracer_option(struct trace_array *tr, char *cmp, int neg)
{
	struct tracer *trace = tr->current_trace;
	struct tracer_flags *tracer_flags = trace->flags;
	struct tracer_opt *opts = NULL;
	int i;

	for (i = 0; tracer_flags->opts[i].name; i++) {
		opts = &tracer_flags->opts[i];

		if (strcmp(cmp, opts->name) == 0)
			return __set_tracer_option(tr, trace->flags, opts, neg);
	}

	return -EINVAL;
}