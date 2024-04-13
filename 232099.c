static
void bfq_release_process_ref(struct bfq_data *bfqd, struct bfq_queue *bfqq)
{
	/*
	 * To prevent bfqq's service guarantees from being violated,
	 * bfqq may be left busy, i.e., queued for service, even if
	 * empty (see comments in __bfq_bfqq_expire() for
	 * details). But, if no process will send requests to bfqq any
	 * longer, then there is no point in keeping bfqq queued for
	 * service. In addition, keeping bfqq queued for service, but
	 * with no process ref any longer, may have caused bfqq to be
	 * freed when dequeued from service. But this is assumed to
	 * never happen.
	 */
	if (bfq_bfqq_busy(bfqq) && RB_EMPTY_ROOT(&bfqq->sort_list) &&
	    bfqq != bfqd->in_service_queue)
		bfq_del_bfqq_busy(bfqd, bfqq, false);

	bfq_put_queue(bfqq);