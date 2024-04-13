tracing_stats_read(struct file *filp, char __user *ubuf,
		   size_t count, loff_t *ppos)
{
	struct inode *inode = file_inode(filp);
	struct trace_array *tr = inode->i_private;
	struct trace_buffer *trace_buf = &tr->trace_buffer;
	int cpu = tracing_get_cpu(inode);
	struct trace_seq *s;
	unsigned long cnt;
	unsigned long long t;
	unsigned long usec_rem;

	s = kmalloc(sizeof(*s), GFP_KERNEL);
	if (!s)
		return -ENOMEM;

	trace_seq_init(s);

	cnt = ring_buffer_entries_cpu(trace_buf->buffer, cpu);
	trace_seq_printf(s, "entries: %ld\n", cnt);

	cnt = ring_buffer_overrun_cpu(trace_buf->buffer, cpu);
	trace_seq_printf(s, "overrun: %ld\n", cnt);

	cnt = ring_buffer_commit_overrun_cpu(trace_buf->buffer, cpu);
	trace_seq_printf(s, "commit overrun: %ld\n", cnt);

	cnt = ring_buffer_bytes_cpu(trace_buf->buffer, cpu);
	trace_seq_printf(s, "bytes: %ld\n", cnt);

	if (trace_clocks[tr->clock_id].in_ns) {
		/* local or global for trace_clock */
		t = ns2usecs(ring_buffer_oldest_event_ts(trace_buf->buffer, cpu));
		usec_rem = do_div(t, USEC_PER_SEC);
		trace_seq_printf(s, "oldest event ts: %5llu.%06lu\n",
								t, usec_rem);

		t = ns2usecs(ring_buffer_time_stamp(trace_buf->buffer, cpu));
		usec_rem = do_div(t, USEC_PER_SEC);
		trace_seq_printf(s, "now ts: %5llu.%06lu\n", t, usec_rem);
	} else {
		/* counter or tsc mode for trace_clock */
		trace_seq_printf(s, "oldest event ts: %llu\n",
				ring_buffer_oldest_event_ts(trace_buf->buffer, cpu));

		trace_seq_printf(s, "now ts: %llu\n",
				ring_buffer_time_stamp(trace_buf->buffer, cpu));
	}

	cnt = ring_buffer_dropped_events_cpu(trace_buf->buffer, cpu);
	trace_seq_printf(s, "dropped events: %ld\n", cnt);

	cnt = ring_buffer_read_events_cpu(trace_buf->buffer, cpu);
	trace_seq_printf(s, "read events: %ld\n", cnt);

	count = simple_read_from_buffer(ubuf, count, ppos,
					s->buffer, trace_seq_used(s));

	kfree(s);

	return count;
}