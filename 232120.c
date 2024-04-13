 */
void bfq_bfqq_expire(struct bfq_data *bfqd,
		     struct bfq_queue *bfqq,
		     bool compensate,
		     enum bfqq_expiration reason)
{
	bool slow;
	unsigned long delta = 0;
	struct bfq_entity *entity = &bfqq->entity;

	/*
	 * Check whether the process is slow (see bfq_bfqq_is_slow).
	 */
	slow = bfq_bfqq_is_slow(bfqd, bfqq, compensate, reason, &delta);

	/*
	 * As above explained, charge slow (typically seeky) and
	 * timed-out queues with the time and not the service
	 * received, to favor sequential workloads.
	 *
	 * Processes doing I/O in the slower disk zones will tend to
	 * be slow(er) even if not seeky. Therefore, since the
	 * estimated peak rate is actually an average over the disk
	 * surface, these processes may timeout just for bad luck. To
	 * avoid punishing them, do not charge time to processes that
	 * succeeded in consuming at least 2/3 of their budget. This
	 * allows BFQ to preserve enough elasticity to still perform
	 * bandwidth, and not time, distribution with little unlucky
	 * or quasi-sequential processes.
	 */
	if (bfqq->wr_coeff == 1 &&
	    (slow ||
	     (reason == BFQQE_BUDGET_TIMEOUT &&
	      bfq_bfqq_budget_left(bfqq) >=  entity->budget / 3)))
		bfq_bfqq_charge_time(bfqd, bfqq, delta);

	if (reason == BFQQE_TOO_IDLE &&
	    entity->service <= 2 * entity->budget / 10)
		bfq_clear_bfqq_IO_bound(bfqq);

	if (bfqd->low_latency && bfqq->wr_coeff == 1)
		bfqq->last_wr_start_finish = jiffies;

	if (bfqd->low_latency && bfqd->bfq_wr_max_softrt_rate > 0 &&
	    RB_EMPTY_ROOT(&bfqq->sort_list)) {
		/*
		 * If we get here, and there are no outstanding
		 * requests, then the request pattern is isochronous
		 * (see the comments on the function
		 * bfq_bfqq_softrt_next_start()). Thus we can compute
		 * soft_rt_next_start. And we do it, unless bfqq is in
		 * interactive weight raising. We do not do it in the
		 * latter subcase, for the following reason. bfqq may
		 * be conveying the I/O needed to load a soft
		 * real-time application. Such an application will
		 * actually exhibit a soft real-time I/O pattern after
		 * it finally starts doing its job. But, if
		 * soft_rt_next_start is computed here for an
		 * interactive bfqq, and bfqq had received a lot of
		 * service before remaining with no outstanding
		 * request (likely to happen on a fast device), then
		 * soft_rt_next_start would be assigned such a high
		 * value that, for a very long time, bfqq would be
		 * prevented from being possibly considered as soft
		 * real time.
		 *
		 * If, instead, the queue still has outstanding
		 * requests, then we have to wait for the completion
		 * of all the outstanding requests to discover whether
		 * the request pattern is actually isochronous.
		 */
		if (bfqq->dispatched == 0 &&
		    bfqq->wr_coeff != bfqd->bfq_wr_coeff)
			bfqq->soft_rt_next_start =
				bfq_bfqq_softrt_next_start(bfqd, bfqq);
		else if (bfqq->dispatched > 0) {
			/*
			 * Schedule an update of soft_rt_next_start to when
			 * the task may be discovered to be isochronous.
			 */
			bfq_mark_bfqq_softrt_update(bfqq);
		}
	}

	bfq_log_bfqq(bfqd, bfqq,
		"expire (%d, slow %d, num_disp %d, short_ttime %d)", reason,
		slow, bfqq->dispatched, bfq_bfqq_has_short_ttime(bfqq));

	/*
	 * bfqq expired, so no total service time needs to be computed
	 * any longer: reset state machine for measuring total service
	 * times.
	 */
	bfqd->rqs_injected = bfqd->wait_dispatch = false;
	bfqd->waited_rq = NULL;

	/*
	 * Increase, decrease or leave budget unchanged according to
	 * reason.
	 */
	__bfq_bfqq_recalc_budget(bfqd, bfqq, reason);
	if (__bfq_bfqq_expire(bfqd, bfqq, reason))
		/* bfqq is gone, no more actions on it */
		return;

	/* mark bfqq as waiting a request only if a bic still points to it */
	if (!bfq_bfqq_busy(bfqq) &&
	    reason != BFQQE_BUDGET_TIMEOUT &&
	    reason != BFQQE_BUDGET_EXHAUSTED) {
		bfq_mark_bfqq_non_blocking_wait_rq(bfqq);
		/*
		 * Not setting service to 0, because, if the next rq
		 * arrives in time, the queue will go on receiving
		 * service with this same budget (as if it never expired)
		 */
	} else
		entity->service = 0;

	/*
	 * Reset the received-service counter for every parent entity.
	 * Differently from what happens with bfqq->entity.service,
	 * the resetting of this counter never needs to be postponed
	 * for parent entities. In fact, in case bfqq may have a
	 * chance to go on being served using the last, partially
	 * consumed budget, bfqq->entity.service needs to be kept,
	 * because if bfqq then actually goes on being served using
	 * the same budget, the last value of bfqq->entity.service is
	 * needed to properly decrement bfqq->entity.budget by the
	 * portion already consumed. In contrast, it is not necessary
	 * to keep entity->service for parent entities too, because
	 * the bubble up of the new value of bfqq->entity.budget will
	 * make sure that the budgets of parent entities are correct,
	 * even in case bfqq and thus parent entities go on receiving
	 * service with the same budget.
	 */
	entity = entity->parent;
	for_each_entity(entity)
		entity->service = 0;