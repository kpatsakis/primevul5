 */
static bool bfq_bfqq_update_budg_for_activation(struct bfq_data *bfqd,
						struct bfq_queue *bfqq,
						bool arrived_in_time)
{
	struct bfq_entity *entity = &bfqq->entity;

	/*
	 * In the next compound condition, we check also whether there
	 * is some budget left, because otherwise there is no point in
	 * trying to go on serving bfqq with this same budget: bfqq
	 * would be expired immediately after being selected for
	 * service. This would only cause useless overhead.
	 */
	if (bfq_bfqq_non_blocking_wait_rq(bfqq) && arrived_in_time &&
	    bfq_bfqq_budget_left(bfqq) > 0) {
		/*
		 * We do not clear the flag non_blocking_wait_rq here, as
		 * the latter is used in bfq_activate_bfqq to signal
		 * that timestamps need to be back-shifted (and is
		 * cleared right after).
		 */

		/*
		 * In next assignment we rely on that either
		 * entity->service or entity->budget are not updated
		 * on expiration if bfqq is empty (see
		 * __bfq_bfqq_recalc_budget). Thus both quantities
		 * remain unchanged after such an expiration, and the
		 * following statement therefore assigns to
		 * entity->budget the remaining budget on such an
		 * expiration.
		 */
		entity->budget = min_t(unsigned long,
				       bfq_bfqq_budget_left(bfqq),
				       bfqq->max_budget);

		/*
		 * At this point, we have used entity->service to get
		 * the budget left (needed for updating
		 * entity->budget). Thus we finally can, and have to,
		 * reset entity->service. The latter must be reset
		 * because bfqq would otherwise be charged again for
		 * the service it has received during its previous
		 * service slot(s).
		 */
		entity->service = 0;

		return true;
	}

	/*
	 * We can finally complete expiration, by setting service to 0.
	 */
	entity->service = 0;
	entity->budget = max_t(unsigned long, bfqq->max_budget,
			       bfq_serv_to_charge(bfqq->next_rq, bfqq));
	bfq_clear_bfqq_non_blocking_wait_rq(bfqq);
	return false;