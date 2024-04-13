 */
void bfq_weights_tree_remove(struct bfq_data *bfqd,
			     struct bfq_queue *bfqq)
{
	struct bfq_entity *entity = bfqq->entity.parent;

	for_each_entity(entity) {
		struct bfq_sched_data *sd = entity->my_sched_data;

		if (sd->next_in_service || sd->in_service_entity) {
			/*
			 * entity is still active, because either
			 * next_in_service or in_service_entity is not
			 * NULL (see the comments on the definition of
			 * next_in_service for details on why
			 * in_service_entity must be checked too).
			 *
			 * As a consequence, its parent entities are
			 * active as well, and thus this loop must
			 * stop here.
			 */
			break;
		}

		/*
		 * The decrement of num_groups_with_pending_reqs is
		 * not performed immediately upon the deactivation of
		 * entity, but it is delayed to when it also happens
		 * that the first leaf descendant bfqq of entity gets
		 * all its pending requests completed. The following
		 * instructions perform this delayed decrement, if
		 * needed. See the comments on
		 * num_groups_with_pending_reqs for details.
		 */
		if (entity->in_groups_with_pending_reqs) {
			entity->in_groups_with_pending_reqs = false;
			bfqd->num_groups_with_pending_reqs--;
		}
	}

	/*
	 * Next function is invoked last, because it causes bfqq to be
	 * freed if the following holds: bfqq is not in service and
	 * has no dispatched request. DO NOT use bfqq after the next
	 * function invocation.
	 */
	__bfq_weights_tree_remove(bfqd, bfqq,
				  &bfqd->queue_weights_tree);