void trace_event_buffer_commit(struct trace_event_buffer *fbuffer)
{
	if (static_key_false(&tracepoint_printk_key.key))
		output_printk(fbuffer);

	event_trigger_unlock_commit(fbuffer->trace_file, fbuffer->buffer,
				    fbuffer->event, fbuffer->entry,
				    fbuffer->flags, fbuffer->pc);
}