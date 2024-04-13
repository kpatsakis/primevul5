 */
static bool idling_needed_for_service_guarantees(struct bfq_data *bfqd,
						 struct bfq_queue *bfqq)
{
	/* No point in idling for bfqq if it won't get requests any longer */
	if (unlikely(!bfqq_process_refs(bfqq)))
		return false;

	return (bfqq->wr_coeff > 1 &&
		(bfqd->wr_busy_queues <
		 bfq_tot_busy_queues(bfqd) ||
		 bfqd->rq_in_driver >=
		 bfqq->dispatched + 4)) ||
		bfq_asymmetric_scenario(bfqd, bfqq);