tracing_fill_pipe_page(size_t rem, struct trace_iterator *iter)
{
	size_t count;
	int save_len;
	int ret;

	/* Seq buffer is page-sized, exactly what we need. */
	for (;;) {
		save_len = iter->seq.seq.len;
		ret = print_trace_line(iter);

		if (trace_seq_has_overflowed(&iter->seq)) {
			iter->seq.seq.len = save_len;
			break;
		}

		/*
		 * This should not be hit, because it should only
		 * be set if the iter->seq overflowed. But check it
		 * anyway to be safe.
		 */
		if (ret == TRACE_TYPE_PARTIAL_LINE) {
			iter->seq.seq.len = save_len;
			break;
		}

		count = trace_seq_used(&iter->seq) - save_len;
		if (rem < count) {
			rem = 0;
			iter->seq.seq.len = save_len;
			break;
		}

		if (ret != TRACE_TYPE_NO_CONSUME)
			trace_consume(iter);
		rem -= count;
		if (!trace_find_next_entry_inc(iter))	{
			rem = 0;
			iter->ent = NULL;
			break;
		}
	}

	return rem;
}