
static void bfq_end_wr(struct bfq_data *bfqd)
{
	struct bfq_queue *bfqq;

	spin_lock_irq(&bfqd->lock);

	list_for_each_entry(bfqq, &bfqd->active_list, bfqq_list)
		bfq_bfqq_end_wr(bfqq);
	list_for_each_entry(bfqq, &bfqd->idle_list, bfqq_list)
		bfq_bfqq_end_wr(bfqq);
	bfq_end_wr_async(bfqd);

	spin_unlock_irq(&bfqd->lock);