tracing_max_lat_read(struct file *filp, char __user *ubuf,
		     size_t cnt, loff_t *ppos)
{
	return tracing_nsecs_read(filp->private_data, ubuf, cnt, ppos);
}