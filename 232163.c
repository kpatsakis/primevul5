 */
void bfq_weights_tree_add(struct bfq_data *bfqd, struct bfq_queue *bfqq,
			  struct rb_root_cached *root)
{
	struct bfq_entity *entity = &bfqq->entity;
	struct rb_node **new = &(root->rb_root.rb_node), *parent = NULL;
	bool leftmost = true;

	/*
	 * Do not insert if the queue is already associated with a
	 * counter, which happens if:
	 *   1) a request arrival has caused the queue to become both
	 *      non-weight-raised, and hence change its weight, and
	 *      backlogged; in this respect, each of the two events
	 *      causes an invocation of this function,
	 *   2) this is the invocation of this function caused by the
	 *      second event. This second invocation is actually useless,
	 *      and we handle this fact by exiting immediately. More
	 *      efficient or clearer solutions might possibly be adopted.
	 */
	if (bfqq->weight_counter)
		return;

	while (*new) {
		struct bfq_weight_counter *__counter = container_of(*new,
						struct bfq_weight_counter,
						weights_node);
		parent = *new;

		if (entity->weight == __counter->weight) {
			bfqq->weight_counter = __counter;
			goto inc_counter;
		}
		if (entity->weight < __counter->weight)
			new = &((*new)->rb_left);
		else {
			new = &((*new)->rb_right);
			leftmost = false;
		}
	}

	bfqq->weight_counter = kzalloc(sizeof(struct bfq_weight_counter),
				       GFP_ATOMIC);

	/*
	 * In the unlucky event of an allocation failure, we just
	 * exit. This will cause the weight of queue to not be
	 * considered in bfq_asymmetric_scenario, which, in its turn,
	 * causes the scenario to be deemed wrongly symmetric in case
	 * bfqq's weight would have been the only weight making the
	 * scenario asymmetric.  On the bright side, no unbalance will
	 * however occur when bfqq becomes inactive again (the
	 * invocation of this function is triggered by an activation
	 * of queue).  In fact, bfq_weights_tree_remove does nothing
	 * if !bfqq->weight_counter.
	 */
	if (unlikely(!bfqq->weight_counter))
		return;

	bfqq->weight_counter->weight = entity->weight;
	rb_link_node(&bfqq->weight_counter->weights_node, parent, new);
	rb_insert_color_cached(&bfqq->weight_counter->weights_node, root,
				leftmost);

inc_counter:
	bfqq->weight_counter->num_active++;
	bfqq->ref++;