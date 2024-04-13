
static void bfq_bfqq_handle_idle_busy_switch(struct bfq_data *bfqd,
					     struct bfq_queue *bfqq,
					     int old_wr_coeff,
					     struct request *rq,
					     bool *interactive)
{
	bool soft_rt, in_burst,	wr_or_deserves_wr,
		bfqq_wants_to_preempt,
		idle_for_long_time = bfq_bfqq_idle_for_long_time(bfqd, bfqq),
		/*
		 * See the comments on
		 * bfq_bfqq_update_budg_for_activation for
		 * details on the usage of the next variable.
		 */
		arrived_in_time =  ktime_get_ns() <=
			bfqq->ttime.last_end_request +
			bfqd->bfq_slice_idle * 3;


	/*
	 * bfqq deserves to be weight-raised if:
	 * - it is sync,
	 * - it does not belong to a large burst,
	 * - it has been idle for enough time or is soft real-time,
	 * - is linked to a bfq_io_cq (it is not shared in any sense).
	 */
	in_burst = bfq_bfqq_in_large_burst(bfqq);
	soft_rt = bfqd->bfq_wr_max_softrt_rate > 0 &&
		!BFQQ_TOTALLY_SEEKY(bfqq) &&
		!in_burst &&
		time_is_before_jiffies(bfqq->soft_rt_next_start) &&
		bfqq->dispatched == 0;
	*interactive = !in_burst && idle_for_long_time;
	wr_or_deserves_wr = bfqd->low_latency &&
		(bfqq->wr_coeff > 1 ||
		 (bfq_bfqq_sync(bfqq) &&
		  bfqq->bic && (*interactive || soft_rt)));

	/*
	 * Using the last flag, update budget and check whether bfqq
	 * may want to preempt the in-service queue.
	 */
	bfqq_wants_to_preempt =
		bfq_bfqq_update_budg_for_activation(bfqd, bfqq,
						    arrived_in_time);

	/*
	 * If bfqq happened to be activated in a burst, but has been
	 * idle for much more than an interactive queue, then we
	 * assume that, in the overall I/O initiated in the burst, the
	 * I/O associated with bfqq is finished. So bfqq does not need
	 * to be treated as a queue belonging to a burst
	 * anymore. Accordingly, we reset bfqq's in_large_burst flag
	 * if set, and remove bfqq from the burst list if it's
	 * there. We do not decrement burst_size, because the fact
	 * that bfqq does not need to belong to the burst list any
	 * more does not invalidate the fact that bfqq was created in
	 * a burst.
	 */
	if (likely(!bfq_bfqq_just_created(bfqq)) &&
	    idle_for_long_time &&
	    time_is_before_jiffies(
		    bfqq->budget_timeout +
		    msecs_to_jiffies(10000))) {
		hlist_del_init(&bfqq->burst_list_node);
		bfq_clear_bfqq_in_large_burst(bfqq);
	}

	bfq_clear_bfqq_just_created(bfqq);


	if (!bfq_bfqq_IO_bound(bfqq)) {
		if (arrived_in_time) {
			bfqq->requests_within_timer++;
			if (bfqq->requests_within_timer >=
			    bfqd->bfq_requests_within_timer)
				bfq_mark_bfqq_IO_bound(bfqq);
		} else
			bfqq->requests_within_timer = 0;
	}

	if (bfqd->low_latency) {
		if (unlikely(time_is_after_jiffies(bfqq->split_time)))
			/* wraparound */
			bfqq->split_time =
				jiffies - bfqd->bfq_wr_min_idle_time - 1;

		if (time_is_before_jiffies(bfqq->split_time +
					   bfqd->bfq_wr_min_idle_time)) {
			bfq_update_bfqq_wr_on_rq_arrival(bfqd, bfqq,
							 old_wr_coeff,
							 wr_or_deserves_wr,
							 *interactive,
							 in_burst,
							 soft_rt);

			if (old_wr_coeff != bfqq->wr_coeff)
				bfqq->entity.prio_changed = 1;
		}
	}

	bfqq->last_idle_bklogged = jiffies;
	bfqq->service_from_backlogged = 0;
	bfq_clear_bfqq_softrt_update(bfqq);

	bfq_add_bfqq_busy(bfqd, bfqq);

	/*
	 * Expire in-service queue only if preemption may be needed
	 * for guarantees. In particular, we care only about two
	 * cases. The first is that bfqq has to recover a service
	 * hole, as explained in the comments on
	 * bfq_bfqq_update_budg_for_activation(), i.e., that
	 * bfqq_wants_to_preempt is true. However, if bfqq does not
	 * carry time-critical I/O, then bfqq's bandwidth is less
	 * important than that of queues that carry time-critical I/O.
	 * So, as a further constraint, we consider this case only if
	 * bfqq is at least as weight-raised, i.e., at least as time
	 * critical, as the in-service queue.
	 *
	 * The second case is that bfqq is in a higher priority class,
	 * or has a higher weight than the in-service queue. If this
	 * condition does not hold, we don't care because, even if
	 * bfqq does not start to be served immediately, the resulting
	 * delay for bfqq's I/O is however lower or much lower than
	 * the ideal completion time to be guaranteed to bfqq's I/O.
	 *
	 * In both cases, preemption is needed only if, according to
	 * the timestamps of both bfqq and of the in-service queue,
	 * bfqq actually is the next queue to serve. So, to reduce
	 * useless preemptions, the return value of
	 * next_queue_may_preempt() is considered in the next compound
	 * condition too. Yet next_queue_may_preempt() just checks a
	 * simple, necessary condition for bfqq to be the next queue
	 * to serve. In fact, to evaluate a sufficient condition, the
	 * timestamps of the in-service queue would need to be
	 * updated, and this operation is quite costly (see the
	 * comments on bfq_bfqq_update_budg_for_activation()).
	 */
	if (bfqd->in_service_queue &&
	    ((bfqq_wants_to_preempt &&
	      bfqq->wr_coeff >= bfqd->in_service_queue->wr_coeff) ||
	     bfq_bfqq_higher_class_or_weight(bfqq, bfqd->in_service_queue)) &&
	    next_queue_may_preempt(bfqd))
		bfq_bfqq_expire(bfqd, bfqd->in_service_queue,
				false, BFQQE_PREEMPTED);