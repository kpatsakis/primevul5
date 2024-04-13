tracing_cpumask_read(struct file *filp, char __user *ubuf,
		     size_t count, loff_t *ppos)
{
	struct trace_array *tr = file_inode(filp)->i_private;
	char *mask_str;
	int len;

	len = snprintf(NULL, 0, "%*pb\n",
		       cpumask_pr_args(tr->tracing_cpumask)) + 1;
	mask_str = kmalloc(len, GFP_KERNEL);
	if (!mask_str)
		return -ENOMEM;

	len = snprintf(mask_str, len, "%*pb\n",
		       cpumask_pr_args(tr->tracing_cpumask));
	if (len >= count) {
		count = -EINVAL;
		goto out_err;
	}
	count = simple_read_from_buffer(ubuf, count, ppos, mask_str, len);

out_err:
	kfree(mask_str);

	return count;
}