static int tracing_snapshot_open(struct inode *inode, struct file *file)
{
	struct trace_array *tr = inode->i_private;
	struct trace_iterator *iter;
	struct seq_file *m;
	int ret = 0;

	if (trace_array_get(tr) < 0)
		return -ENODEV;

	if (file->f_mode & FMODE_READ) {
		iter = __tracing_open(inode, file, true);
		if (IS_ERR(iter))
			ret = PTR_ERR(iter);
	} else {
		/* Writes still need the seq_file to hold the private data */
		ret = -ENOMEM;
		m = kzalloc(sizeof(*m), GFP_KERNEL);
		if (!m)
			goto out;
		iter = kzalloc(sizeof(*iter), GFP_KERNEL);
		if (!iter) {
			kfree(m);
			goto out;
		}
		ret = 0;

		iter->tr = tr;
		iter->trace_buffer = &tr->max_buffer;
		iter->cpu_file = tracing_get_cpu(inode);
		m->private = iter;
		file->private_data = m;
	}
out:
	if (ret < 0)
		trace_array_put(tr);

	return ret;
}