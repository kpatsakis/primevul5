tracing_buffers_read(struct file *filp, char __user *ubuf,
		     size_t count, loff_t *ppos)
{
	struct ftrace_buffer_info *info = filp->private_data;
	struct trace_iterator *iter = &info->iter;
	ssize_t ret = 0;
	ssize_t size;

	if (!count)
		return 0;

#ifdef CONFIG_TRACER_MAX_TRACE
	if (iter->snapshot && iter->tr->current_trace->use_max_tr)
		return -EBUSY;
#endif

	if (!info->spare) {
		info->spare = ring_buffer_alloc_read_page(iter->trace_buffer->buffer,
							  iter->cpu_file);
		if (IS_ERR(info->spare)) {
			ret = PTR_ERR(info->spare);
			info->spare = NULL;
		} else {
			info->spare_cpu = iter->cpu_file;
		}
	}
	if (!info->spare)
		return ret;

	/* Do we have previous read data to read? */
	if (info->read < PAGE_SIZE)
		goto read;

 again:
	trace_access_lock(iter->cpu_file);
	ret = ring_buffer_read_page(iter->trace_buffer->buffer,
				    &info->spare,
				    count,
				    iter->cpu_file, 0);
	trace_access_unlock(iter->cpu_file);

	if (ret < 0) {
		if (trace_empty(iter)) {
			if ((filp->f_flags & O_NONBLOCK))
				return -EAGAIN;

			ret = wait_on_pipe(iter, false);
			if (ret)
				return ret;

			goto again;
		}
		return 0;
	}

	info->read = 0;
 read:
	size = PAGE_SIZE - info->read;
	if (size > count)
		size = count;

	ret = copy_to_user(ubuf, info->spare + info->read, size);
	if (ret == size)
		return -EFAULT;

	size -= ret;

	*ppos += size;
	info->read += size;

	return size;
}