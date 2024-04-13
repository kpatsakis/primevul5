static void free_saved_cmdlines_buffer(struct saved_cmdlines_buffer *s)
{
	kfree(s->saved_cmdlines);
	kfree(s->map_cmdline_to_pid);
	kfree(s);
}