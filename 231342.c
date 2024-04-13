int tracing_alloc_snapshot(void)
{
	WARN_ONCE(1, "Snapshot feature not enabled, but snapshot allocation used");
	return -ENODEV;
}