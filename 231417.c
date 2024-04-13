static ssize_t tracing_clock_write(struct file *filp, const char __user *ubuf,
				   size_t cnt, loff_t *fpos)
{
	struct seq_file *m = filp->private_data;
	struct trace_array *tr = m->private;
	char buf[64];
	const char *clockstr;
	int ret;

	if (cnt >= sizeof(buf))
		return -EINVAL;

	if (copy_from_user(buf, ubuf, cnt))
		return -EFAULT;

	buf[cnt] = 0;

	clockstr = strstrip(buf);

	ret = tracing_set_clock(tr, clockstr);
	if (ret)
		return ret;

	*fpos += cnt;

	return cnt;
}