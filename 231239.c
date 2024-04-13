tracing_saved_cmdlines_size_read(struct file *filp, char __user *ubuf,
				 size_t cnt, loff_t *ppos)
{
	char buf[64];
	int r;

	arch_spin_lock(&trace_cmdline_lock);
	r = scnprintf(buf, sizeof(buf), "%u\n", savedcmd->cmdline_num);
	arch_spin_unlock(&trace_cmdline_lock);

	return simple_read_from_buffer(ubuf, cnt, ppos, buf, r);
}