/* Empty burst list and add just bfqq (see comments on bfq_handle_burst) */
static void bfq_reset_burst_list(struct bfq_data *bfqd, struct bfq_queue *bfqq)
{
	struct bfq_queue *item;
	struct hlist_node *n;

	hlist_for_each_entry_safe(item, n, &bfqd->burst_list, burst_list_node)
		hlist_del_init(&item->burst_list_node);

	/*
	 * Start the creation of a new burst list only if there is no
	 * active queue. See comments on the conditional invocation of
	 * bfq_handle_burst().
	 */
	if (bfq_tot_busy_queues(bfqd) == 0) {
		hlist_add_head(&bfqq->burst_list_node, &bfqd->burst_list);
		bfqd->burst_size = 1;
	} else
		bfqd->burst_size = 0;

	bfqd->burst_parent_entity = bfqq->entity.parent;