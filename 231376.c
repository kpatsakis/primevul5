static int tracing_release(struct inode *inode, struct file *file)
{
	struct trace_array *tr = inode->i_private;
	struct seq_file *m = file->private_data;
	struct trace_iterator *iter;
	int cpu;

	if (!(file->f_mode & FMODE_READ)) {
		trace_array_put(tr);
		return 0;
	}

	/* Writes do not use seq_file */
	iter = m->private;
	mutex_lock(&trace_types_lock);

	for_each_tracing_cpu(cpu) {
		if (iter->buffer_iter[cpu])
			ring_buffer_read_finish(iter->buffer_iter[cpu]);
	}

	if (iter->trace && iter->trace->close)
		iter->trace->close(iter);

	if (!iter->snapshot)
		/* reenable tracing if it was previously enabled */
		tracing_start_tr(tr);

	__trace_array_put(tr);

	mutex_unlock(&trace_types_lock);

	mutex_destroy(&iter->mutex);
	free_cpumask_var(iter->started);
	kfree(iter->trace);
	kfree(iter->buffer_iter);
	seq_release_private(inode, file);

	return 0;
}