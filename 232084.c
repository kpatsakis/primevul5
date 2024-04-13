 */
void bfq_put_queue(struct bfq_queue *bfqq)
{
	struct bfq_queue *item;
	struct hlist_node *n;
	struct bfq_group *bfqg = bfqq_group(bfqq);

	if (bfqq->bfqd)
		bfq_log_bfqq(bfqq->bfqd, bfqq, "put_queue: %p %d",
			     bfqq, bfqq->ref);

	bfqq->ref--;
	if (bfqq->ref)
		return;

	if (!hlist_unhashed(&bfqq->burst_list_node)) {
		hlist_del_init(&bfqq->burst_list_node);
		/*
		 * Decrement also burst size after the removal, if the
		 * process associated with bfqq is exiting, and thus
		 * does not contribute to the burst any longer. This
		 * decrement helps filter out false positives of large
		 * bursts, when some short-lived process (often due to
		 * the execution of commands by some service) happens
		 * to start and exit while a complex application is
		 * starting, and thus spawning several processes that
		 * do I/O (and that *must not* be treated as a large
		 * burst, see comments on bfq_handle_burst).
		 *
		 * In particular, the decrement is performed only if:
		 * 1) bfqq is not a merged queue, because, if it is,
		 * then this free of bfqq is not triggered by the exit
		 * of the process bfqq is associated with, but exactly
		 * by the fact that bfqq has just been merged.
		 * 2) burst_size is greater than 0, to handle
		 * unbalanced decrements. Unbalanced decrements may
		 * happen in te following case: bfqq is inserted into
		 * the current burst list--without incrementing
		 * bust_size--because of a split, but the current
		 * burst list is not the burst list bfqq belonged to
		 * (see comments on the case of a split in
		 * bfq_set_request).
		 */
		if (bfqq->bic && bfqq->bfqd->burst_size > 0)
			bfqq->bfqd->burst_size--;
	}

	/*
	 * bfqq does not exist any longer, so it cannot be woken by
	 * any other queue, and cannot wake any other queue. Then bfqq
	 * must be removed from the woken list of its possible waker
	 * queue, and all queues in the woken list of bfqq must stop
	 * having a waker queue. Strictly speaking, these updates
	 * should be performed when bfqq remains with no I/O source
	 * attached to it, which happens before bfqq gets freed. In
	 * particular, this happens when the last process associated
	 * with bfqq exits or gets associated with a different
	 * queue. However, both events lead to bfqq being freed soon,
	 * and dangling references would come out only after bfqq gets
	 * freed. So these updates are done here, as a simple and safe
	 * way to handle all cases.
	 */
	/* remove bfqq from woken list */
	if (!hlist_unhashed(&bfqq->woken_list_node))
		hlist_del_init(&bfqq->woken_list_node);

	/* reset waker for all queues in woken list */
	hlist_for_each_entry_safe(item, n, &bfqq->woken_list,
				  woken_list_node) {
		item->waker_bfqq = NULL;
		bfq_clear_bfqq_has_waker(item);
		hlist_del_init(&item->woken_list_node);
	}

	if (bfqq->bfqd && bfqq->bfqd->last_completed_rq_bfqq == bfqq)
		bfqq->bfqd->last_completed_rq_bfqq = NULL;

	kmem_cache_free(bfq_pool, bfqq);
	bfqg_and_blkg_put(bfqg);