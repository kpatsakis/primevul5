
static void bfq_update_bfqq_wr_on_rq_arrival(struct bfq_data *bfqd,
					     struct bfq_queue *bfqq,
					     unsigned int old_wr_coeff,
					     bool wr_or_deserves_wr,
					     bool interactive,
					     bool in_burst,
					     bool soft_rt)
{
	if (old_wr_coeff == 1 && wr_or_deserves_wr) {
		/* start a weight-raising period */
		if (interactive) {
			bfqq->service_from_wr = 0;
			bfqq->wr_coeff = bfqd->bfq_wr_coeff;
			bfqq->wr_cur_max_time = bfq_wr_duration(bfqd);
		} else {
			/*
			 * No interactive weight raising in progress
			 * here: assign minus infinity to
			 * wr_start_at_switch_to_srt, to make sure
			 * that, at the end of the soft-real-time
			 * weight raising periods that is starting
			 * now, no interactive weight-raising period
			 * may be wrongly considered as still in
			 * progress (and thus actually started by
			 * mistake).
			 */
			bfqq->wr_start_at_switch_to_srt =
				bfq_smallest_from_now();
			bfqq->wr_coeff = bfqd->bfq_wr_coeff *
				BFQ_SOFTRT_WEIGHT_FACTOR;
			bfqq->wr_cur_max_time =
				bfqd->bfq_wr_rt_max_time;
		}

		/*
		 * If needed, further reduce budget to make sure it is
		 * close to bfqq's backlog, so as to reduce the
		 * scheduling-error component due to a too large
		 * budget. Do not care about throughput consequences,
		 * but only about latency. Finally, do not assign a
		 * too small budget either, to avoid increasing
		 * latency by causing too frequent expirations.
		 */
		bfqq->entity.budget = min_t(unsigned long,
					    bfqq->entity.budget,
					    2 * bfq_min_budget(bfqd));
	} else if (old_wr_coeff > 1) {
		if (interactive) { /* update wr coeff and duration */
			bfqq->wr_coeff = bfqd->bfq_wr_coeff;
			bfqq->wr_cur_max_time = bfq_wr_duration(bfqd);
		} else if (in_burst)
			bfqq->wr_coeff = 1;
		else if (soft_rt) {
			/*
			 * The application is now or still meeting the
			 * requirements for being deemed soft rt.  We
			 * can then correctly and safely (re)charge
			 * the weight-raising duration for the
			 * application with the weight-raising
			 * duration for soft rt applications.
			 *
			 * In particular, doing this recharge now, i.e.,
			 * before the weight-raising period for the
			 * application finishes, reduces the probability
			 * of the following negative scenario:
			 * 1) the weight of a soft rt application is
			 *    raised at startup (as for any newly
			 *    created application),
			 * 2) since the application is not interactive,
			 *    at a certain time weight-raising is
			 *    stopped for the application,
			 * 3) at that time the application happens to
			 *    still have pending requests, and hence
			 *    is destined to not have a chance to be
			 *    deemed soft rt before these requests are
			 *    completed (see the comments to the
			 *    function bfq_bfqq_softrt_next_start()
			 *    for details on soft rt detection),
			 * 4) these pending requests experience a high
			 *    latency because the application is not
			 *    weight-raised while they are pending.
			 */
			if (bfqq->wr_cur_max_time !=
				bfqd->bfq_wr_rt_max_time) {
				bfqq->wr_start_at_switch_to_srt =
					bfqq->last_wr_start_finish;

				bfqq->wr_cur_max_time =
					bfqd->bfq_wr_rt_max_time;
				bfqq->wr_coeff = bfqd->bfq_wr_coeff *
					BFQ_SOFTRT_WEIGHT_FACTOR;
			}
			bfqq->last_wr_start_finish = jiffies;
		}
	}