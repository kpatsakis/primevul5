static int __set_tracer_option(struct trace_array *tr,
			       struct tracer_flags *tracer_flags,
			       struct tracer_opt *opts, int neg)
{
	struct tracer *trace = tracer_flags->trace;
	int ret;

	ret = trace->set_flag(tr, tracer_flags->val, opts->bit, !neg);
	if (ret)
		return ret;

	if (neg)
		tracer_flags->val &= ~opts->bit;
	else
		tracer_flags->val |= opts->bit;
	return 0;
}