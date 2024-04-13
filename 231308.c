static int tracing_open(struct inode *inode, struct file *file)
{
	struct trace_array *tr = inode->i_private;
	struct trace_iterator *iter;
	int ret = 0;

	if (trace_array_get(tr) < 0)
		return -ENODEV;

	/* If this file was open for write, then erase contents */
	if ((file->f_mode & FMODE_WRITE) && (file->f_flags & O_TRUNC)) {
		int cpu = tracing_get_cpu(inode);
		struct trace_buffer *trace_buf = &tr->trace_buffer;

#ifdef CONFIG_TRACER_MAX_TRACE
		if (tr->current_trace->print_max)
			trace_buf = &tr->max_buffer;
#endif

		if (cpu == RING_BUFFER_ALL_CPUS)
			tracing_reset_online_cpus(trace_buf);
		else
			tracing_reset(trace_buf, cpu);
	}

	if (file->f_mode & FMODE_READ) {
		iter = __tracing_open(inode, file, false);
		if (IS_ERR(iter))
			ret = PTR_ERR(iter);
		else if (tr->trace_flags & TRACE_ITER_LATENCY_FMT)
			iter->iter_flags |= TRACE_FILE_LAT_FMT;
	}

	if (ret < 0)
		trace_array_put(tr);

	return ret;
}