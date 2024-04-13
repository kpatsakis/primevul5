int set_tracer_flag(struct trace_array *tr, unsigned int mask, int enabled)
{
	/* do nothing if flag is already set */
	if (!!(tr->trace_flags & mask) == !!enabled)
		return 0;

	/* Give the tracer a chance to approve the change */
	if (tr->current_trace->flag_changed)
		if (tr->current_trace->flag_changed(tr, mask, !!enabled))
			return -EINVAL;

	if (enabled)
		tr->trace_flags |= mask;
	else
		tr->trace_flags &= ~mask;

	if (mask == TRACE_ITER_RECORD_CMD)
		trace_event_enable_cmd_record(enabled);

	if (mask == TRACE_ITER_RECORD_TGID) {
		if (!tgid_map)
			tgid_map = kzalloc((PID_MAX_DEFAULT + 1) * sizeof(*tgid_map),
					   GFP_KERNEL);
		if (!tgid_map) {
			tr->trace_flags &= ~TRACE_ITER_RECORD_TGID;
			return -ENOMEM;
		}

		trace_event_enable_tgid_record(enabled);
	}

	if (mask == TRACE_ITER_EVENT_FORK)
		trace_event_follow_fork(tr, enabled);

	if (mask == TRACE_ITER_FUNC_FORK)
		ftrace_pid_follow_fork(tr, enabled);

	if (mask == TRACE_ITER_OVERWRITE) {
		ring_buffer_change_overwrite(tr->trace_buffer.buffer, enabled);
#ifdef CONFIG_TRACER_MAX_TRACE
		ring_buffer_change_overwrite(tr->max_buffer.buffer, enabled);
#endif
	}

	if (mask == TRACE_ITER_PRINTK) {
		trace_printk_start_stop_comm(enabled);
		trace_printk_control(enabled);
	}

	return 0;
}