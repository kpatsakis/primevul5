static int ep_poll(struct eventpoll *ep, struct epoll_event __user *events,
		   int maxevents, long timeout)
{
	int res = 0, eavail, timed_out = 0;
	u64 slack = 0;
	wait_queue_entry_t wait;
	ktime_t expires, *to = NULL;

	lockdep_assert_irqs_enabled();

	if (timeout > 0) {
		struct timespec64 end_time = ep_set_mstimeout(timeout);

		slack = select_estimate_accuracy(&end_time);
		to = &expires;
		*to = timespec64_to_ktime(end_time);
	} else if (timeout == 0) {
		/*
		 * Avoid the unnecessary trip to the wait queue loop, if the
		 * caller specified a non blocking operation. We still need
		 * lock because we could race and not see an epi being added
		 * to the ready list while in irq callback. Thus incorrectly
		 * returning 0 back to userspace.
		 */
		timed_out = 1;

		write_lock_irq(&ep->lock);
		eavail = ep_events_available(ep);
		write_unlock_irq(&ep->lock);

		goto send_events;
	}

fetch_events:

	if (!ep_events_available(ep))
		ep_busy_loop(ep, timed_out);

	eavail = ep_events_available(ep);
	if (eavail)
		goto send_events;

	/*
	 * Busy poll timed out.  Drop NAPI ID for now, we can add
	 * it back in when we have moved a socket with a valid NAPI
	 * ID onto the ready list.
	 */
	ep_reset_busy_poll_napi_id(ep);

	do {
		/*
		 * Internally init_wait() uses autoremove_wake_function(),
		 * thus wait entry is removed from the wait queue on each
		 * wakeup. Why it is important? In case of several waiters
		 * each new wakeup will hit the next waiter, giving it the
		 * chance to harvest new event. Otherwise wakeup can be
		 * lost. This is also good performance-wise, because on
		 * normal wakeup path no need to call __remove_wait_queue()
		 * explicitly, thus ep->lock is not taken, which halts the
		 * event delivery.
		 */
		init_wait(&wait);

		write_lock_irq(&ep->lock);
		/*
		 * Barrierless variant, waitqueue_active() is called under
		 * the same lock on wakeup ep_poll_callback() side, so it
		 * is safe to avoid an explicit barrier.
		 */
		__set_current_state(TASK_INTERRUPTIBLE);

		/*
		 * Do the final check under the lock. ep_scan_ready_list()
		 * plays with two lists (->rdllist and ->ovflist) and there
		 * is always a race when both lists are empty for short
		 * period of time although events are pending, so lock is
		 * important.
		 */
		eavail = ep_events_available(ep);
		if (!eavail) {
			if (signal_pending(current))
				res = -EINTR;
			else
				__add_wait_queue_exclusive(&ep->wq, &wait);
		}
		write_unlock_irq(&ep->lock);

		if (eavail || res)
			break;

		if (!schedule_hrtimeout_range(to, slack, HRTIMER_MODE_ABS)) {
			timed_out = 1;
			break;
		}

		/* We were woken up, thus go and try to harvest some events */
		eavail = 1;

	} while (0);

	__set_current_state(TASK_RUNNING);

	if (!list_empty_careful(&wait.entry)) {
		write_lock_irq(&ep->lock);
		__remove_wait_queue(&ep->wq, &wait);
		write_unlock_irq(&ep->lock);
	}

send_events:
	if (fatal_signal_pending(current)) {
		/*
		 * Always short-circuit for fatal signals to allow
		 * threads to make a timely exit without the chance of
		 * finding more events available and fetching
		 * repeatedly.
		 */
		res = -EINTR;
	}
	/*
	 * Try to transfer events to user space. In case we get 0 events and
	 * there's still timeout left over, we go trying again in search of
	 * more luck.
	 */
	if (!res && eavail &&
	    !(res = ep_send_events(ep, events, maxevents)) && !timed_out)
		goto fetch_events;

	return res;
}