tracing_max_lat_write(struct file *filp, const char __user *ubuf,
		      size_t cnt, loff_t *ppos)
{
	return tracing_nsecs_write(filp->private_data, ubuf, cnt, ppos);
}