 */
static void bfq_update_inject_limit(struct bfq_data *bfqd,
				    struct bfq_queue *bfqq)
{
	u64 tot_time_ns = ktime_get_ns() - bfqd->last_empty_occupied_ns;
	unsigned int old_limit = bfqq->inject_limit;

	if (bfqq->last_serv_time_ns > 0 && bfqd->rqs_injected) {
		u64 threshold = (bfqq->last_serv_time_ns * 3)>>1;

		if (tot_time_ns >= threshold && old_limit > 0) {
			bfqq->inject_limit--;
			bfqq->decrease_time_jif = jiffies;
		} else if (tot_time_ns < threshold &&
			   old_limit <= bfqd->max_rq_in_driver)
			bfqq->inject_limit++;
	}

	/*
	 * Either we still have to compute the base value for the
	 * total service time, and there seem to be the right
	 * conditions to do it, or we can lower the last base value
	 * computed.
	 *
	 * NOTE: (bfqd->rq_in_driver == 1) means that there is no I/O
	 * request in flight, because this function is in the code
	 * path that handles the completion of a request of bfqq, and,
	 * in particular, this function is executed before
	 * bfqd->rq_in_driver is decremented in such a code path.
	 */
	if ((bfqq->last_serv_time_ns == 0 && bfqd->rq_in_driver == 1) ||
	    tot_time_ns < bfqq->last_serv_time_ns) {
		if (bfqq->last_serv_time_ns == 0) {
			/*
			 * Now we certainly have a base value: make sure we
			 * start trying injection.
			 */
			bfqq->inject_limit = max_t(unsigned int, 1, old_limit);
		}
		bfqq->last_serv_time_ns = tot_time_ns;
	} else if (!bfqd->rqs_injected && bfqd->rq_in_driver == 1)
		/*
		 * No I/O injected and no request still in service in
		 * the drive: these are the exact conditions for
		 * computing the base value of the total service time
		 * for bfqq. So let's update this value, because it is
		 * rather variable. For example, it varies if the size
		 * or the spatial locality of the I/O requests in bfqq
		 * change.
		 */
		bfqq->last_serv_time_ns = tot_time_ns;


	/* update complete, not waiting for any request completion any longer */
	bfqd->waited_rq = NULL;
	bfqd->rqs_injected = false;