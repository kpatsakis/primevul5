void tracing_snapshot_alloc(void)
{
	int ret;

	ret = tracing_alloc_snapshot();
	if (ret < 0)
		return;

	tracing_snapshot();
}