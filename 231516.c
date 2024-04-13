tracing_read_dyn_info(struct file *filp, char __user *ubuf,
		  size_t cnt, loff_t *ppos)
{
	unsigned long *p = filp->private_data;
	char buf[64]; /* Not too big for a shallow stack */
	int r;

	r = scnprintf(buf, 63, "%ld", *p);
	buf[r++] = '\n';

	return simple_read_from_buffer(ubuf, cnt, ppos, buf, r);
}