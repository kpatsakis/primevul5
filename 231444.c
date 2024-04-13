tracing_trace_options_write(struct file *filp, const char __user *ubuf,
			size_t cnt, loff_t *ppos)
{
	struct seq_file *m = filp->private_data;
	struct trace_array *tr = m->private;
	char buf[64];
	int ret;

	if (cnt >= sizeof(buf))
		return -EINVAL;

	if (copy_from_user(buf, ubuf, cnt))
		return -EFAULT;

	buf[cnt] = 0;

	ret = trace_set_options(tr, buf);
	if (ret < 0)
		return ret;

	*ppos += cnt;

	return cnt;
}