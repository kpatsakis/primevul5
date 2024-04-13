struct dentry *trace_create_file(const char *name,
				 umode_t mode,
				 struct dentry *parent,
				 void *data,
				 const struct file_operations *fops)
{
	struct dentry *ret;

	ret = tracefs_create_file(name, mode, parent, data, fops);
	if (!ret)
		pr_warn("Could not create tracefs '%s' entry\n", name);

	return ret;
}