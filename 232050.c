 */
static bool bfq_asymmetric_scenario(struct bfq_data *bfqd,
				   struct bfq_queue *bfqq)
{
	bool smallest_weight = bfqq &&
		bfqq->weight_counter &&
		bfqq->weight_counter ==
		container_of(
			rb_first_cached(&bfqd->queue_weights_tree),
			struct bfq_weight_counter,
			weights_node);

	/*
	 * For queue weights to differ, queue_weights_tree must contain
	 * at least two nodes.
	 */
	bool varied_queue_weights = !smallest_weight &&
		!RB_EMPTY_ROOT(&bfqd->queue_weights_tree.rb_root) &&
		(bfqd->queue_weights_tree.rb_root.rb_node->rb_left ||
		 bfqd->queue_weights_tree.rb_root.rb_node->rb_right);

	bool multiple_classes_busy =
		(bfqd->busy_queues[0] && bfqd->busy_queues[1]) ||
		(bfqd->busy_queues[0] && bfqd->busy_queues[2]) ||
		(bfqd->busy_queues[1] && bfqd->busy_queues[2]);

	return varied_queue_weights || multiple_classes_busy
#ifdef CONFIG_BFQ_GROUP_IOSCHED
	       || bfqd->num_groups_with_pending_reqs > 0
#endif
		;