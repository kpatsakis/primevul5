int trace_keep_overwrite(struct tracer *tracer, u32 mask, int set)
{
	if (tracer->enabled && (mask & TRACE_ITER_OVERWRITE) && !set)
		return -1;

	return 0;
}