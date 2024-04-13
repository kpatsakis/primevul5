int call_filter_check_discard(struct trace_event_call *call, void *rec,
			      struct ring_buffer *buffer,
			      struct ring_buffer_event *event)
{
	if (unlikely(call->flags & TRACE_EVENT_FL_FILTERED) &&
	    !filter_match_preds(call->filter, rec)) {
		__trace_event_discard_commit(buffer, event);
		return 1;
	}

	return 0;
}