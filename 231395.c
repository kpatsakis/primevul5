static void output_printk(struct trace_event_buffer *fbuffer)
{
	struct trace_event_call *event_call;
	struct trace_event *event;
	unsigned long flags;
	struct trace_iterator *iter = tracepoint_print_iter;

	/* We should never get here if iter is NULL */
	if (WARN_ON_ONCE(!iter))
		return;

	event_call = fbuffer->trace_file->event_call;
	if (!event_call || !event_call->event.funcs ||
	    !event_call->event.funcs->trace)
		return;

	event = &fbuffer->trace_file->event_call->event;

	spin_lock_irqsave(&tracepoint_iter_lock, flags);
	trace_seq_init(&iter->seq);
	iter->ent = fbuffer->entry;
	event_call->event.funcs->trace(iter, 0, event);
	trace_seq_putc(&iter->seq, 0);
	printk("%s", iter->seq.buffer);

	spin_unlock_irqrestore(&tracepoint_iter_lock, flags);
}