tracing_buffers_splice_read(struct file *file, loff_t *ppos,
			    struct pipe_inode_info *pipe, size_t len,
			    unsigned int flags)
{
	struct ftrace_buffer_info *info = file->private_data;
	struct trace_iterator *iter = &info->iter;
	struct partial_page partial_def[PIPE_DEF_BUFFERS];
	struct page *pages_def[PIPE_DEF_BUFFERS];
	struct splice_pipe_desc spd = {
		.pages		= pages_def,
		.partial	= partial_def,
		.nr_pages_max	= PIPE_DEF_BUFFERS,
		.ops		= &buffer_pipe_buf_ops,
		.spd_release	= buffer_spd_release,
	};
	struct buffer_ref *ref;
	int entries, i;
	ssize_t ret = 0;

#ifdef CONFIG_TRACER_MAX_TRACE
	if (iter->snapshot && iter->tr->current_trace->use_max_tr)
		return -EBUSY;
#endif

	if (*ppos & (PAGE_SIZE - 1))
		return -EINVAL;

	if (len & (PAGE_SIZE - 1)) {
		if (len < PAGE_SIZE)
			return -EINVAL;
		len &= PAGE_MASK;
	}

	if (splice_grow_spd(pipe, &spd))
		return -ENOMEM;

 again:
	trace_access_lock(iter->cpu_file);
	entries = ring_buffer_entries_cpu(iter->trace_buffer->buffer, iter->cpu_file);

	for (i = 0; i < spd.nr_pages_max && len && entries; i++, len -= PAGE_SIZE) {
		struct page *page;
		int r;

		ref = kzalloc(sizeof(*ref), GFP_KERNEL);
		if (!ref) {
			ret = -ENOMEM;
			break;
		}

		ref->ref = 1;
		ref->buffer = iter->trace_buffer->buffer;
		ref->page = ring_buffer_alloc_read_page(ref->buffer, iter->cpu_file);
		if (IS_ERR(ref->page)) {
			ret = PTR_ERR(ref->page);
			ref->page = NULL;
			kfree(ref);
			break;
		}
		ref->cpu = iter->cpu_file;

		r = ring_buffer_read_page(ref->buffer, &ref->page,
					  len, iter->cpu_file, 1);
		if (r < 0) {
			ring_buffer_free_read_page(ref->buffer, ref->cpu,
						   ref->page);
			kfree(ref);
			break;
		}

		page = virt_to_page(ref->page);

		spd.pages[i] = page;
		spd.partial[i].len = PAGE_SIZE;
		spd.partial[i].offset = 0;
		spd.partial[i].private = (unsigned long)ref;
		spd.nr_pages++;
		*ppos += PAGE_SIZE;

		entries = ring_buffer_entries_cpu(iter->trace_buffer->buffer, iter->cpu_file);
	}

	trace_access_unlock(iter->cpu_file);
	spd.nr_pages = i;

	/* did we read anything? */
	if (!spd.nr_pages) {
		if (ret)
			goto out;

		ret = -EAGAIN;
		if ((file->f_flags & O_NONBLOCK) || (flags & SPLICE_F_NONBLOCK))
			goto out;

		ret = wait_on_pipe(iter, true);
		if (ret)
			goto out;

		goto again;
	}

	ret = splice_to_pipe(pipe, &spd);
out:
	splice_shrink_spd(&spd);

	return ret;
}