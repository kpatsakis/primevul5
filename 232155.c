#ifdef CONFIG_BFQ_CGROUP_DEBUG
static void bfq_update_insert_stats(struct request_queue *q,
				    struct bfq_queue *bfqq,
				    bool idle_timer_disabled,
				    unsigned int cmd_flags)
{
	if (!bfqq)
		return;

	/*
	 * bfqq still exists, because it can disappear only after
	 * either it is merged with another queue, or the process it
	 * is associated with exits. But both actions must be taken by
	 * the same process currently executing this flow of
	 * instructions.
	 *
	 * In addition, the following queue lock guarantees that
	 * bfqq_group(bfqq) exists as well.
	 */
	spin_lock_irq(&q->queue_lock);
	bfqg_stats_update_io_add(bfqq_group(bfqq), bfqq, cmd_flags);
	if (idle_timer_disabled)
		bfqg_stats_update_idle_time(bfqq_group(bfqq));
	spin_unlock_irq(&q->queue_lock);