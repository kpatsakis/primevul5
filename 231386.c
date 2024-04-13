trace_event_setup(struct ring_buffer_event *event,
		  int type, unsigned long flags, int pc)
{
	struct trace_entry *ent = ring_buffer_event_data(event);

	tracing_generic_entry_update(ent, flags, pc);
	ent->type = type;
}