tracing_thresh_write(struct file *filp, const char __user *ubuf,
		     size_t cnt, loff_t *ppos)
{
	struct trace_array *tr = filp->private_data;
	int ret;

	mutex_lock(&trace_types_lock);
	ret = tracing_nsecs_write(&tracing_thresh, ubuf, cnt, ppos);
	if (ret < 0)
		goto out;

	if (tr->current_trace->update_thresh) {
		ret = tr->current_trace->update_thresh(tr);
		if (ret < 0)
			goto out;
	}

	ret = cnt;
out:
	mutex_unlock(&trace_types_lock);

	return ret;
}