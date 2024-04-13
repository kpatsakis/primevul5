
static void bfq_add_request(struct request *rq)
{
	struct bfq_queue *bfqq = RQ_BFQQ(rq);
	struct bfq_data *bfqd = bfqq->bfqd;
	struct request *next_rq, *prev;
	unsigned int old_wr_coeff = bfqq->wr_coeff;
	bool interactive = false;

	bfq_log_bfqq(bfqd, bfqq, "add_request %d", rq_is_sync(rq));
	bfqq->queued[rq_is_sync(rq)]++;
	bfqd->queued++;

	if (RB_EMPTY_ROOT(&bfqq->sort_list) && bfq_bfqq_sync(bfqq)) {
		/*
		 * Detect whether bfqq's I/O seems synchronized with
		 * that of some other queue, i.e., whether bfqq, after
		 * remaining empty, happens to receive new I/O only
		 * right after some I/O request of the other queue has
		 * been completed. We call waker queue the other
		 * queue, and we assume, for simplicity, that bfqq may
		 * have at most one waker queue.
		 *
		 * A remarkable throughput boost can be reached by
		 * unconditionally injecting the I/O of the waker
		 * queue, every time a new bfq_dispatch_request
		 * happens to be invoked while I/O is being plugged
		 * for bfqq.  In addition to boosting throughput, this
		 * unblocks bfqq's I/O, thereby improving bandwidth
		 * and latency for bfqq. Note that these same results
		 * may be achieved with the general injection
		 * mechanism, but less effectively. For details on
		 * this aspect, see the comments on the choice of the
		 * queue for injection in bfq_select_queue().
		 *
		 * Turning back to the detection of a waker queue, a
		 * queue Q is deemed as a waker queue for bfqq if, for
		 * two consecutive times, bfqq happens to become non
		 * empty right after a request of Q has been
		 * completed. In particular, on the first time, Q is
		 * tentatively set as a candidate waker queue, while
		 * on the second time, the flag
		 * bfq_bfqq_has_waker(bfqq) is set to confirm that Q
		 * is a waker queue for bfqq. These detection steps
		 * are performed only if bfqq has a long think time,
		 * so as to make it more likely that bfqq's I/O is
		 * actually being blocked by a synchronization. This
		 * last filter, plus the above two-times requirement,
		 * make false positives less likely.
		 *
		 * NOTE
		 *
		 * The sooner a waker queue is detected, the sooner
		 * throughput can be boosted by injecting I/O from the
		 * waker queue. Fortunately, detection is likely to be
		 * actually fast, for the following reasons. While
		 * blocked by synchronization, bfqq has a long think
		 * time. This implies that bfqq's inject limit is at
		 * least equal to 1 (see the comments in
		 * bfq_update_inject_limit()). So, thanks to
		 * injection, the waker queue is likely to be served
		 * during the very first I/O-plugging time interval
		 * for bfqq. This triggers the first step of the
		 * detection mechanism. Thanks again to injection, the
		 * candidate waker queue is then likely to be
		 * confirmed no later than during the next
		 * I/O-plugging interval for bfqq.
		 */
		if (bfqd->last_completed_rq_bfqq &&
		    !bfq_bfqq_has_short_ttime(bfqq) &&
		    ktime_get_ns() - bfqd->last_completion <
		    200 * NSEC_PER_USEC) {
			if (bfqd->last_completed_rq_bfqq != bfqq &&
			    bfqd->last_completed_rq_bfqq !=
			    bfqq->waker_bfqq) {
				/*
				 * First synchronization detected with
				 * a candidate waker queue, or with a
				 * different candidate waker queue
				 * from the current one.
				 */
				bfqq->waker_bfqq = bfqd->last_completed_rq_bfqq;

				/*
				 * If the waker queue disappears, then
				 * bfqq->waker_bfqq must be reset. To
				 * this goal, we maintain in each
				 * waker queue a list, woken_list, of
				 * all the queues that reference the
				 * waker queue through their
				 * waker_bfqq pointer. When the waker
				 * queue exits, the waker_bfqq pointer
				 * of all the queues in the woken_list
				 * is reset.
				 *
				 * In addition, if bfqq is already in
				 * the woken_list of a waker queue,
				 * then, before being inserted into
				 * the woken_list of a new waker
				 * queue, bfqq must be removed from
				 * the woken_list of the old waker
				 * queue.
				 */
				if (!hlist_unhashed(&bfqq->woken_list_node))
					hlist_del_init(&bfqq->woken_list_node);
				hlist_add_head(&bfqq->woken_list_node,
				    &bfqd->last_completed_rq_bfqq->woken_list);

				bfq_clear_bfqq_has_waker(bfqq);
			} else if (bfqd->last_completed_rq_bfqq ==
				   bfqq->waker_bfqq &&
				   !bfq_bfqq_has_waker(bfqq)) {
				/*
				 * synchronization with waker_bfqq
				 * seen for the second time
				 */
				bfq_mark_bfqq_has_waker(bfqq);
			}
		}

		/*
		 * Periodically reset inject limit, to make sure that
		 * the latter eventually drops in case workload
		 * changes, see step (3) in the comments on
		 * bfq_update_inject_limit().
		 */
		if (time_is_before_eq_jiffies(bfqq->decrease_time_jif +
					     msecs_to_jiffies(1000)))
			bfq_reset_inject_limit(bfqd, bfqq);

		/*
		 * The following conditions must hold to setup a new
		 * sampling of total service time, and then a new
		 * update of the inject limit:
		 * - bfqq is in service, because the total service
		 *   time is evaluated only for the I/O requests of
		 *   the queues in service;
		 * - this is the right occasion to compute or to
		 *   lower the baseline total service time, because
		 *   there are actually no requests in the drive,
		 *   or
		 *   the baseline total service time is available, and
		 *   this is the right occasion to compute the other
		 *   quantity needed to update the inject limit, i.e.,
		 *   the total service time caused by the amount of
		 *   injection allowed by the current value of the
		 *   limit. It is the right occasion because injection
		 *   has actually been performed during the service
		 *   hole, and there are still in-flight requests,
		 *   which are very likely to be exactly the injected
		 *   requests, or part of them;
		 * - the minimum interval for sampling the total
		 *   service time and updating the inject limit has
		 *   elapsed.
		 */
		if (bfqq == bfqd->in_service_queue &&
		    (bfqd->rq_in_driver == 0 ||
		     (bfqq->last_serv_time_ns > 0 &&
		      bfqd->rqs_injected && bfqd->rq_in_driver > 0)) &&
		    time_is_before_eq_jiffies(bfqq->decrease_time_jif +
					      msecs_to_jiffies(10))) {
			bfqd->last_empty_occupied_ns = ktime_get_ns();
			/*
			 * Start the state machine for measuring the
			 * total service time of rq: setting
			 * wait_dispatch will cause bfqd->waited_rq to
			 * be set when rq will be dispatched.
			 */
			bfqd->wait_dispatch = true;
			/*
			 * If there is no I/O in service in the drive,
			 * then possible injection occurred before the
			 * arrival of rq will not affect the total
			 * service time of rq. So the injection limit
			 * must not be updated as a function of such
			 * total service time, unless new injection
			 * occurs before rq is completed. To have the
			 * injection limit updated only in the latter
			 * case, reset rqs_injected here (rqs_injected
			 * will be set in case injection is performed
			 * on bfqq before rq is completed).
			 */
			if (bfqd->rq_in_driver == 0)
				bfqd->rqs_injected = false;
		}
	}

	elv_rb_add(&bfqq->sort_list, rq);

	/*
	 * Check if this request is a better next-serve candidate.
	 */
	prev = bfqq->next_rq;
	next_rq = bfq_choose_req(bfqd, bfqq->next_rq, rq, bfqd->last_position);
	bfqq->next_rq = next_rq;

	/*
	 * Adjust priority tree position, if next_rq changes.
	 * See comments on bfq_pos_tree_add_move() for the unlikely().
	 */
	if (unlikely(!bfqd->nonrot_with_queueing && prev != bfqq->next_rq))
		bfq_pos_tree_add_move(bfqd, bfqq);

	if (!bfq_bfqq_busy(bfqq)) /* switching to busy ... */
		bfq_bfqq_handle_idle_busy_switch(bfqd, bfqq, old_wr_coeff,
						 rq, &interactive);
	else {
		if (bfqd->low_latency && old_wr_coeff == 1 && !rq_is_sync(rq) &&
		    time_is_before_jiffies(
				bfqq->last_wr_start_finish +
				bfqd->bfq_wr_min_inter_arr_async)) {
			bfqq->wr_coeff = bfqd->bfq_wr_coeff;
			bfqq->wr_cur_max_time = bfq_wr_duration(bfqd);

			bfqd->wr_busy_queues++;
			bfqq->entity.prio_changed = 1;
		}
		if (prev != bfqq->next_rq)
			bfq_updated_next_req(bfqd, bfqq);
	}

	/*
	 * Assign jiffies to last_wr_start_finish in the following
	 * cases:
	 *
	 * . if bfqq is not going to be weight-raised, because, for
	 *   non weight-raised queues, last_wr_start_finish stores the
	 *   arrival time of the last request; as of now, this piece
	 *   of information is used only for deciding whether to
	 *   weight-raise async queues
	 *
	 * . if bfqq is not weight-raised, because, if bfqq is now
	 *   switching to weight-raised, then last_wr_start_finish
	 *   stores the time when weight-raising starts
	 *
	 * . if bfqq is interactive, because, regardless of whether
	 *   bfqq is currently weight-raised, the weight-raising
	 *   period must start or restart (this case is considered
	 *   separately because it is not detected by the above
	 *   conditions, if bfqq is already weight-raised)
	 *
	 * last_wr_start_finish has to be updated also if bfqq is soft
	 * real-time, because the weight-raising period is constantly
	 * restarted on idle-to-busy transitions for these queues, but
	 * this is already done in bfq_bfqq_handle_idle_busy_switch if
	 * needed.
	 */
	if (bfqd->low_latency &&
		(old_wr_coeff == 1 || bfqq->wr_coeff == 1 || interactive))
		bfqq->last_wr_start_finish = jiffies;