static int tracing_snapshot_release(struct inode *inode, struct file *file)
{
	struct seq_file *m = file->private_data;
	int ret;

	ret = tracing_release(inode, file);

	if (file->f_mode & FMODE_READ)
		return ret;

	/* If write only, the seq_file is just a stub */
	if (m)
		kfree(m->private);
	kfree(m);

	return 0;
}