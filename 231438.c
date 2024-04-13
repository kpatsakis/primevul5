tracing_thresh_read(struct file *filp, char __user *ubuf,
		    size_t cnt, loff_t *ppos)
{
	return tracing_nsecs_read(&tracing_thresh, ubuf, cnt, ppos);
}