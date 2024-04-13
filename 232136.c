
static bool bfq_too_late_for_merging(struct bfq_queue *bfqq)
{
	return bfqq->service_from_backlogged > 0 &&
		time_is_before_jiffies(bfqq->first_IO_time +
				       bfq_merge_time_limit);