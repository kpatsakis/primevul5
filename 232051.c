#ifdef CONFIG_BFQ_CGROUP_DEBUG
static void bfq_update_dispatch_stats(struct request_queue *q,
				      struct request *rq,
				      struct bfq_queue *in_serv_queue,
				      bool idle_timer_disabled)
{
	struct bfq_queue *bfqq = rq ? RQ_BFQQ(rq) : NULL;

	if (!idle_timer_disabled && !bfqq)
		return;

	/*
	 * rq and bfqq are guaranteed to exist until this function
	 * ends, for the following reasons. First, rq can be
	 * dispatched to the device, and then can be completed and
	 * freed, only after this function ends. Second, rq cannot be
	 * merged (and thus freed because of a merge) any longer,
	 * because it has already started. Thus rq cannot be freed
	 * before this function ends, and, since rq has a reference to
	 * bfqq, the same guarantee holds for bfqq too.
	 *
	 * In addition, the following queue lock guarantees that
	 * bfqq_group(bfqq) exists as well.
	 */
	spin_lock_irq(&q->queue_lock);
	if (idle_timer_disabled)
		/*
		 * Since the idle timer has been disabled,
		 * in_serv_queue contained some request when
		 * __bfq_dispatch_request was invoked above, which
		 * implies that rq was picked exactly from
		 * in_serv_queue. Thus in_serv_queue == bfqq, and is
		 * therefore guaranteed to exist because of the above
		 * arguments.
		 */
		bfqg_stats_update_idle_time(bfqq_group(in_serv_queue));
	if (bfqq) {
		struct bfq_group *bfqg = bfqq_group(bfqq);

		bfqg_stats_update_avg_queue_size(bfqg);
		bfqg_stats_set_start_empty_time(bfqg);
		bfqg_stats_update_io_remove(bfqg, rq->cmd_flags);
	}
	spin_unlock_irq(&q->queue_lock);