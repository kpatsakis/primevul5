static struct bfq_queue *
bfq_choose_bfqq_for_injection(struct bfq_data *bfqd)
{
	struct bfq_queue *bfqq, *in_serv_bfqq = bfqd->in_service_queue;
	unsigned int limit = in_serv_bfqq->inject_limit;
	/*
	 * If
	 * - bfqq is not weight-raised and therefore does not carry
	 *   time-critical I/O,
	 * or
	 * - regardless of whether bfqq is weight-raised, bfqq has
	 *   however a long think time, during which it can absorb the
	 *   effect of an appropriate number of extra I/O requests
	 *   from other queues (see bfq_update_inject_limit for
	 *   details on the computation of this number);
	 * then injection can be performed without restrictions.
	 */
	bool in_serv_always_inject = in_serv_bfqq->wr_coeff == 1 ||
		!bfq_bfqq_has_short_ttime(in_serv_bfqq);

	/*
	 * If
	 * - the baseline total service time could not be sampled yet,
	 *   so the inject limit happens to be still 0, and
	 * - a lot of time has elapsed since the plugging of I/O
	 *   dispatching started, so drive speed is being wasted
	 *   significantly;
	 * then temporarily raise inject limit to one request.
	 */
	if (limit == 0 && in_serv_bfqq->last_serv_time_ns == 0 &&
	    bfq_bfqq_wait_request(in_serv_bfqq) &&
	    time_is_before_eq_jiffies(bfqd->last_idling_start_jiffies +
				      bfqd->bfq_slice_idle)
		)
		limit = 1;

	if (bfqd->rq_in_driver >= limit)
		return NULL;

	/*
	 * Linear search of the source queue for injection; but, with
	 * a high probability, very few steps are needed to find a
	 * candidate queue, i.e., a queue with enough budget left for
	 * its next request. In fact:
	 * - BFQ dynamically updates the budget of every queue so as
	 *   to accommodate the expected backlog of the queue;
	 * - if a queue gets all its requests dispatched as injected
	 *   service, then the queue is removed from the active list
	 *   (and re-added only if it gets new requests, but then it
	 *   is assigned again enough budget for its new backlog).
	 */
	list_for_each_entry(bfqq, &bfqd->active_list, bfqq_list)
		if (!RB_EMPTY_ROOT(&bfqq->sort_list) &&
		    (in_serv_always_inject || bfqq->wr_coeff > 1) &&
		    bfq_serv_to_charge(bfqq->next_rq, bfqq) <=
		    bfq_bfqq_budget_left(bfqq)) {
			/*
			 * Allow for only one large in-flight request
			 * on non-rotational devices, for the
			 * following reason. On non-rotationl drives,
			 * large requests take much longer than
			 * smaller requests to be served. In addition,
			 * the drive prefers to serve large requests
			 * w.r.t. to small ones, if it can choose. So,
			 * having more than one large requests queued
			 * in the drive may easily make the next first
			 * request of the in-service queue wait for so
			 * long to break bfqq's service guarantees. On
			 * the bright side, large requests let the
			 * drive reach a very high throughput, even if
			 * there is only one in-flight large request
			 * at a time.
			 */
			if (blk_queue_nonrot(bfqd->queue) &&
			    blk_rq_sectors(bfqq->next_rq) >=
			    BFQQ_SECT_THR_NONROT)
				limit = min_t(unsigned int, 1, limit);
			else
				limit = in_serv_bfqq->inject_limit;

			if (bfqd->rq_in_driver < limit) {
				bfqd->rqs_injected = true;
				return bfqq;
			}
		}

	return NULL;