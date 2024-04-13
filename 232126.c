
static bool __bfq_bfqq_expire(struct bfq_data *bfqd, struct bfq_queue *bfqq,
			      enum bfqq_expiration reason)
{
	/*
	 * If this bfqq is shared between multiple processes, check
	 * to make sure that those processes are still issuing I/Os
	 * within the mean seek distance. If not, it may be time to
	 * break the queues apart again.
	 */
	if (bfq_bfqq_coop(bfqq) && BFQQ_SEEKY(bfqq))
		bfq_mark_bfqq_split_coop(bfqq);

	/*
	 * Consider queues with a higher finish virtual time than
	 * bfqq. If idling_needed_for_service_guarantees(bfqq) returns
	 * true, then bfqq's bandwidth would be violated if an
	 * uncontrolled amount of I/O from these queues were
	 * dispatched while bfqq is waiting for its new I/O to
	 * arrive. This is exactly what may happen if this is a forced
	 * expiration caused by a preemption attempt, and if bfqq is
	 * not re-scheduled. To prevent this from happening, re-queue
	 * bfqq if it needs I/O-dispatch plugging, even if it is
	 * empty. By doing so, bfqq is granted to be served before the
	 * above queues (provided that bfqq is of course eligible).
	 */
	if (RB_EMPTY_ROOT(&bfqq->sort_list) &&
	    !(reason == BFQQE_PREEMPTED &&
	      idling_needed_for_service_guarantees(bfqd, bfqq))) {
		if (bfqq->dispatched == 0)
			/*
			 * Overloading budget_timeout field to store
			 * the time at which the queue remains with no
			 * backlog and no outstanding request; used by
			 * the weight-raising mechanism.
			 */
			bfqq->budget_timeout = jiffies;

		bfq_del_bfqq_busy(bfqd, bfqq, true);
	} else {
		bfq_requeue_bfqq(bfqd, bfqq, true);
		/*
		 * Resort priority tree of potential close cooperators.
		 * See comments on bfq_pos_tree_add_move() for the unlikely().
		 */
		if (unlikely(!bfqd->nonrot_with_queueing &&
			     !RB_EMPTY_ROOT(&bfqq->sort_list)))
			bfq_pos_tree_add_move(bfqd, bfqq);
	}

	/*
	 * All in-service entities must have been properly deactivated
	 * or requeued before executing the next function, which
	 * resets all in-service entities as no more in service. This
	 * may cause bfqq to be freed. If this happens, the next
	 * function returns true.
	 */
	return __bfq_bfqd_reset_in_service(bfqd);