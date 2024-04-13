trace_options_core_read(struct file *filp, char __user *ubuf, size_t cnt,
			loff_t *ppos)
{
	void *tr_index = filp->private_data;
	struct trace_array *tr;
	unsigned int index;
	char *buf;

	get_tr_index(tr_index, &tr, &index);

	if (tr->trace_flags & (1 << index))
		buf = "1\n";
	else
		buf = "0\n";

	return simple_read_from_buffer(ubuf, cnt, ppos, buf, 2);
}