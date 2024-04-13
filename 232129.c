 */
static bool bfq_bfqq_must_idle(struct bfq_queue *bfqq)
{
	return RB_EMPTY_ROOT(&bfqq->sort_list) && bfq_better_to_idle(bfqq);