static int alloc_snapshot(struct trace_array *tr)
{
	int ret;

	if (!tr->allocated_snapshot) {

		/* allocate spare buffer */
		ret = resize_buffer_duplicate_size(&tr->max_buffer,
				   &tr->trace_buffer, RING_BUFFER_ALL_CPUS);
		if (ret < 0)
			return ret;

		tr->allocated_snapshot = true;
	}

	return 0;
}