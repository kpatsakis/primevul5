 */
static void bfq_update_peak_rate(struct bfq_data *bfqd, struct request *rq)
{
	u64 now_ns = ktime_get_ns();

	if (bfqd->peak_rate_samples == 0) { /* first dispatch */
		bfq_log(bfqd, "update_peak_rate: goto reset, samples %d",
			bfqd->peak_rate_samples);
		bfq_reset_rate_computation(bfqd, rq);
		goto update_last_values; /* will add one sample */
	}

	/*
	 * Device idle for very long: the observation interval lasting
	 * up to this dispatch cannot be a valid observation interval
	 * for computing a new peak rate (similarly to the late-
	 * completion event in bfq_completed_request()). Go to
	 * update_rate_and_reset to have the following three steps
	 * taken:
	 * - close the observation interval at the last (previous)
	 *   request dispatch or completion
	 * - compute rate, if possible, for that observation interval
	 * - start a new observation interval with this dispatch
	 */
	if (now_ns - bfqd->last_dispatch > 100*NSEC_PER_MSEC &&
	    bfqd->rq_in_driver == 0)
		goto update_rate_and_reset;

	/* Update sampling information */
	bfqd->peak_rate_samples++;

	if ((bfqd->rq_in_driver > 0 ||
		now_ns - bfqd->last_completion < BFQ_MIN_TT)
	    && !BFQ_RQ_SEEKY(bfqd, bfqd->last_position, rq))
		bfqd->sequential_samples++;

	bfqd->tot_sectors_dispatched += blk_rq_sectors(rq);

	/* Reset max observed rq size every 32 dispatches */
	if (likely(bfqd->peak_rate_samples % 32))
		bfqd->last_rq_max_size =
			max_t(u32, blk_rq_sectors(rq), bfqd->last_rq_max_size);
	else
		bfqd->last_rq_max_size = blk_rq_sectors(rq);

	bfqd->delta_from_first = now_ns - bfqd->first_dispatch;

	/* Target observation interval not yet reached, go on sampling */
	if (bfqd->delta_from_first < BFQ_RATE_REF_INTERVAL)
		goto update_last_values;

update_rate_and_reset:
	bfq_update_rate_reset(bfqd, rq);
update_last_values:
	bfqd->last_position = blk_rq_pos(rq) + blk_rq_sectors(rq);
	if (RQ_BFQQ(rq) == bfqd->in_service_queue)
		bfqd->in_serv_last_pos = bfqd->last_position;
	bfqd->last_dispatch = now_ns;