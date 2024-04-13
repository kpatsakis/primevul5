create_trace_option_files(struct trace_array *tr, struct tracer *tracer)
{
	struct trace_option_dentry *topts;
	struct trace_options *tr_topts;
	struct tracer_flags *flags;
	struct tracer_opt *opts;
	int cnt;
	int i;

	if (!tracer)
		return;

	flags = tracer->flags;

	if (!flags || !flags->opts)
		return;

	/*
	 * If this is an instance, only create flags for tracers
	 * the instance may have.
	 */
	if (!trace_ok_for_array(tracer, tr))
		return;

	for (i = 0; i < tr->nr_topts; i++) {
		/* Make sure there's no duplicate flags. */
		if (WARN_ON_ONCE(tr->topts[i].tracer->flags == tracer->flags))
			return;
	}

	opts = flags->opts;

	for (cnt = 0; opts[cnt].name; cnt++)
		;

	topts = kcalloc(cnt + 1, sizeof(*topts), GFP_KERNEL);
	if (!topts)
		return;

	tr_topts = krealloc(tr->topts, sizeof(*tr->topts) * (tr->nr_topts + 1),
			    GFP_KERNEL);
	if (!tr_topts) {
		kfree(topts);
		return;
	}

	tr->topts = tr_topts;
	tr->topts[tr->nr_topts].tracer = tracer;
	tr->topts[tr->nr_topts].topts = topts;
	tr->nr_topts++;

	for (cnt = 0; opts[cnt].name; cnt++) {
		create_trace_option_file(tr, &topts[cnt], flags,
					 &opts[cnt]);
		WARN_ONCE(topts[cnt].entry == NULL,
			  "Failed to create trace option: %s",
			  opts[cnt].name);
	}
}