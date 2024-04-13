trace_process_export(struct trace_export *export,
	       struct ring_buffer_event *event)
{
	struct trace_entry *entry;
	unsigned int size = 0;

	entry = ring_buffer_event_data(event);
	size = ring_buffer_event_length(event);
	export->write(export, entry, size);
}