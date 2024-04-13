static int tracing_resize_saved_cmdlines(unsigned int val)
{
	struct saved_cmdlines_buffer *s, *savedcmd_temp;

	s = kmalloc(sizeof(*s), GFP_KERNEL);
	if (!s)
		return -ENOMEM;

	if (allocate_cmdlines_buffer(val, s) < 0) {
		kfree(s);
		return -ENOMEM;
	}

	arch_spin_lock(&trace_cmdline_lock);
	savedcmd_temp = savedcmd;
	savedcmd = s;
	arch_spin_unlock(&trace_cmdline_lock);
	free_saved_cmdlines_buffer(savedcmd_temp);

	return 0;
}