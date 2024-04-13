 */
static struct bfq_queue *bfq_select_queue(struct bfq_data *bfqd)
{
	struct bfq_queue *bfqq;
	struct request *next_rq;
	enum bfqq_expiration reason = BFQQE_BUDGET_TIMEOUT;

	bfqq = bfqd->in_service_queue;
	if (!bfqq)
		goto new_queue;

	bfq_log_bfqq(bfqd, bfqq, "select_queue: already in-service queue");

	/*
	 * Do not expire bfqq for budget timeout if bfqq may be about
	 * to enjoy device idling. The reason why, in this case, we
	 * prevent bfqq from expiring is the same as in the comments
	 * on the case where bfq_bfqq_must_idle() returns true, in
	 * bfq_completed_request().
	 */
	if (bfq_may_expire_for_budg_timeout(bfqq) &&
	    !bfq_bfqq_must_idle(bfqq))
		goto expire;

check_queue:
	/*
	 * This loop is rarely executed more than once. Even when it
	 * happens, it is much more convenient to re-execute this loop
	 * than to return NULL and trigger a new dispatch to get a
	 * request served.
	 */
	next_rq = bfqq->next_rq;
	/*
	 * If bfqq has requests queued and it has enough budget left to
	 * serve them, keep the queue, otherwise expire it.
	 */
	if (next_rq) {
		if (bfq_serv_to_charge(next_rq, bfqq) >
			bfq_bfqq_budget_left(bfqq)) {
			/*
			 * Expire the queue for budget exhaustion,
			 * which makes sure that the next budget is
			 * enough to serve the next request, even if
			 * it comes from the fifo expired path.
			 */
			reason = BFQQE_BUDGET_EXHAUSTED;
			goto expire;
		} else {
			/*
			 * The idle timer may be pending because we may
			 * not disable disk idling even when a new request
			 * arrives.
			 */
			if (bfq_bfqq_wait_request(bfqq)) {
				/*
				 * If we get here: 1) at least a new request
				 * has arrived but we have not disabled the
				 * timer because the request was too small,
				 * 2) then the block layer has unplugged
				 * the device, causing the dispatch to be
				 * invoked.
				 *
				 * Since the device is unplugged, now the
				 * requests are probably large enough to
				 * provide a reasonable throughput.
				 * So we disable idling.
				 */
				bfq_clear_bfqq_wait_request(bfqq);
				hrtimer_try_to_cancel(&bfqd->idle_slice_timer);
			}
			goto keep_queue;
		}
	}

	/*
	 * No requests pending. However, if the in-service queue is idling
	 * for a new request, or has requests waiting for a completion and
	 * may idle after their completion, then keep it anyway.
	 *
	 * Yet, inject service from other queues if it boosts
	 * throughput and is possible.
	 */
	if (bfq_bfqq_wait_request(bfqq) ||
	    (bfqq->dispatched != 0 && bfq_better_to_idle(bfqq))) {
		struct bfq_queue *async_bfqq =
			bfqq->bic && bfqq->bic->bfqq[0] &&
			bfq_bfqq_busy(bfqq->bic->bfqq[0]) &&
			bfqq->bic->bfqq[0]->next_rq ?
			bfqq->bic->bfqq[0] : NULL;

		/*
		 * The next three mutually-exclusive ifs decide
		 * whether to try injection, and choose the queue to
		 * pick an I/O request from.
		 *
		 * The first if checks whether the process associated
		 * with bfqq has also async I/O pending. If so, it
		 * injects such I/O unconditionally. Injecting async
		 * I/O from the same process can cause no harm to the
		 * process. On the contrary, it can only increase
		 * bandwidth and reduce latency for the process.
		 *
		 * The second if checks whether there happens to be a
		 * non-empty waker queue for bfqq, i.e., a queue whose
		 * I/O needs to be completed for bfqq to receive new
		 * I/O. This happens, e.g., if bfqq is associated with
		 * a process that does some sync. A sync generates
		 * extra blocking I/O, which must be completed before
		 * the process associated with bfqq can go on with its
		 * I/O. If the I/O of the waker queue is not served,
		 * then bfqq remains empty, and no I/O is dispatched,
		 * until the idle timeout fires for bfqq. This is
		 * likely to result in lower bandwidth and higher
		 * latencies for bfqq, and in a severe loss of total
		 * throughput. The best action to take is therefore to
		 * serve the waker queue as soon as possible. So do it
		 * (without relying on the third alternative below for
		 * eventually serving waker_bfqq's I/O; see the last
		 * paragraph for further details). This systematic
		 * injection of I/O from the waker queue does not
		 * cause any delay to bfqq's I/O. On the contrary,
		 * next bfqq's I/O is brought forward dramatically,
		 * for it is not blocked for milliseconds.
		 *
		 * The third if checks whether bfqq is a queue for
		 * which it is better to avoid injection. It is so if
		 * bfqq delivers more throughput when served without
		 * any further I/O from other queues in the middle, or
		 * if the service times of bfqq's I/O requests both
		 * count more than overall throughput, and may be
		 * easily increased by injection (this happens if bfqq
		 * has a short think time). If none of these
		 * conditions holds, then a candidate queue for
		 * injection is looked for through
		 * bfq_choose_bfqq_for_injection(). Note that the
		 * latter may return NULL (for example if the inject
		 * limit for bfqq is currently 0).
		 *
		 * NOTE: motivation for the second alternative
		 *
		 * Thanks to the way the inject limit is updated in
		 * bfq_update_has_short_ttime(), it is rather likely
		 * that, if I/O is being plugged for bfqq and the
		 * waker queue has pending I/O requests that are
		 * blocking bfqq's I/O, then the third alternative
		 * above lets the waker queue get served before the
		 * I/O-plugging timeout fires. So one may deem the
		 * second alternative superfluous. It is not, because
		 * the third alternative may be way less effective in
		 * case of a synchronization. For two main
		 * reasons. First, throughput may be low because the
		 * inject limit may be too low to guarantee the same
		 * amount of injected I/O, from the waker queue or
		 * other queues, that the second alternative
		 * guarantees (the second alternative unconditionally
		 * injects a pending I/O request of the waker queue
		 * for each bfq_dispatch_request()). Second, with the
		 * third alternative, the duration of the plugging,
		 * i.e., the time before bfqq finally receives new I/O,
		 * may not be minimized, because the waker queue may
		 * happen to be served only after other queues.
		 */
		if (async_bfqq &&
		    icq_to_bic(async_bfqq->next_rq->elv.icq) == bfqq->bic &&
		    bfq_serv_to_charge(async_bfqq->next_rq, async_bfqq) <=
		    bfq_bfqq_budget_left(async_bfqq))
			bfqq = bfqq->bic->bfqq[0];
		else if (bfq_bfqq_has_waker(bfqq) &&
			   bfq_bfqq_busy(bfqq->waker_bfqq) &&
			   bfqq->next_rq &&
			   bfq_serv_to_charge(bfqq->waker_bfqq->next_rq,
					      bfqq->waker_bfqq) <=
			   bfq_bfqq_budget_left(bfqq->waker_bfqq)
			)
			bfqq = bfqq->waker_bfqq;
		else if (!idling_boosts_thr_without_issues(bfqd, bfqq) &&
			 (bfqq->wr_coeff == 1 || bfqd->wr_busy_queues > 1 ||
			  !bfq_bfqq_has_short_ttime(bfqq)))
			bfqq = bfq_choose_bfqq_for_injection(bfqd);
		else
			bfqq = NULL;

		goto keep_queue;
	}

	reason = BFQQE_NO_MORE_REQUESTS;
expire:
	bfq_bfqq_expire(bfqd, bfqq, false, reason);
new_queue:
	bfqq = bfq_set_in_service_queue(bfqd);
	if (bfqq) {
		bfq_log_bfqq(bfqd, bfqq, "select_queue: checking new queue");
		goto check_queue;
	}
keep_queue:
	if (bfqq)
		bfq_log_bfqq(bfqd, bfqq, "select_queue: returned this queue");
	else
		bfq_log(bfqd, "select_queue: no queue returned");

	return bfqq;