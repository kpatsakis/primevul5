static int ebb_event_check(struct perf_event *event)
{
	struct perf_event *leader = event->group_leader;

	/* Event and group leader must agree on EBB */
	if (is_ebb_event(leader) != is_ebb_event(event))
		return -EINVAL;

	if (is_ebb_event(event)) {
		if (!(event->attach_state & PERF_ATTACH_TASK))
			return -EINVAL;

		if (!leader->attr.pinned || !leader->attr.exclusive)
			return -EINVAL;

		if (event->attr.freq ||
		    event->attr.inherit ||
		    event->attr.sample_type ||
		    event->attr.sample_period ||
		    event->attr.enable_on_exec)
			return -EINVAL;
	}

	return 0;
}