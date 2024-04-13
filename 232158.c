
static void bfq_update_wr_data(struct bfq_data *bfqd, struct bfq_queue *bfqq)
{
	struct bfq_entity *entity = &bfqq->entity;

	if (bfqq->wr_coeff > 1) { /* queue is being weight-raised */
		bfq_log_bfqq(bfqd, bfqq,
			"raising period dur %u/%u msec, old coeff %u, w %d(%d)",
			jiffies_to_msecs(jiffies - bfqq->last_wr_start_finish),
			jiffies_to_msecs(bfqq->wr_cur_max_time),
			bfqq->wr_coeff,
			bfqq->entity.weight, bfqq->entity.orig_weight);

		if (entity->prio_changed)
			bfq_log_bfqq(bfqd, bfqq, "WARN: pending prio change");

		/*
		 * If the queue was activated in a burst, or too much
		 * time has elapsed from the beginning of this
		 * weight-raising period, then end weight raising.
		 */
		if (bfq_bfqq_in_large_burst(bfqq))
			bfq_bfqq_end_wr(bfqq);
		else if (time_is_before_jiffies(bfqq->last_wr_start_finish +
						bfqq->wr_cur_max_time)) {
			if (bfqq->wr_cur_max_time != bfqd->bfq_wr_rt_max_time ||
			time_is_before_jiffies(bfqq->wr_start_at_switch_to_srt +
					       bfq_wr_duration(bfqd)))
				bfq_bfqq_end_wr(bfqq);
			else {
				switch_back_to_interactive_wr(bfqq, bfqd);
				bfqq->entity.prio_changed = 1;
			}
		}
		if (bfqq->wr_coeff > 1 &&
		    bfqq->wr_cur_max_time != bfqd->bfq_wr_rt_max_time &&
		    bfqq->service_from_wr > max_service_from_wr) {
			/* see comments on max_service_from_wr */
			bfq_bfqq_end_wr(bfqq);
		}
	}
	/*
	 * To improve latency (for this or other queues), immediately
	 * update weight both if it must be raised and if it must be
	 * lowered. Since, entity may be on some active tree here, and
	 * might have a pending change of its ioprio class, invoke
	 * next function with the last parameter unset (see the
	 * comments on the function).
	 */
	if ((entity->weight > entity->orig_weight) != (bfqq->wr_coeff > 1))
		__bfq_entity_update_weight_prio(bfq_entity_service_tree(entity),
						entity, false);