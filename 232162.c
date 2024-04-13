
static void bfq_update_hw_tag(struct bfq_data *bfqd)
{
	struct bfq_queue *bfqq = bfqd->in_service_queue;

	bfqd->max_rq_in_driver = max_t(int, bfqd->max_rq_in_driver,
				       bfqd->rq_in_driver);

	if (bfqd->hw_tag == 1)
		return;

	/*
	 * This sample is valid if the number of outstanding requests
	 * is large enough to allow a queueing behavior.  Note that the
	 * sum is not exact, as it's not taking into account deactivated
	 * requests.
	 */
	if (bfqd->rq_in_driver + bfqd->queued <= BFQ_HW_QUEUE_THRESHOLD)
		return;

	/*
	 * If active queue hasn't enough requests and can idle, bfq might not
	 * dispatch sufficient requests to hardware. Don't zero hw_tag in this
	 * case
	 */
	if (bfqq && bfq_bfqq_has_short_ttime(bfqq) &&
	    bfqq->dispatched + bfqq->queued[0] + bfqq->queued[1] <
	    BFQ_HW_QUEUE_THRESHOLD &&
	    bfqd->rq_in_driver < BFQ_HW_QUEUE_THRESHOLD)
		return;

	if (bfqd->hw_tag_samples++ < BFQ_HW_QUEUE_SAMPLES)
		return;

	bfqd->hw_tag = bfqd->max_rq_in_driver > BFQ_HW_QUEUE_THRESHOLD;
	bfqd->max_rq_in_driver = 0;
	bfqd->hw_tag_samples = 0;

	bfqd->nonrot_with_queueing =
		blk_queue_nonrot(bfqd->queue) && bfqd->hw_tag;