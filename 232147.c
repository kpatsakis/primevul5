
static void bfq_reset_inject_limit(struct bfq_data *bfqd,
				   struct bfq_queue *bfqq)
{
	/* invalidate baseline total service time */
	bfqq->last_serv_time_ns = 0;

	/*
	 * Reset pointer in case we are waiting for
	 * some request completion.
	 */
	bfqd->waited_rq = NULL;

	/*
	 * If bfqq has a short think time, then start by setting the
	 * inject limit to 0 prudentially, because the service time of
	 * an injected I/O request may be higher than the think time
	 * of bfqq, and therefore, if one request was injected when
	 * bfqq remains empty, this injected request might delay the
	 * service of the next I/O request for bfqq significantly. In
	 * case bfqq can actually tolerate some injection, then the
	 * adaptive update will however raise the limit soon. This
	 * lucky circumstance holds exactly because bfqq has a short
	 * think time, and thus, after remaining empty, is likely to
	 * get new I/O enqueued---and then completed---before being
	 * expired. This is the very pattern that gives the
	 * limit-update algorithm the chance to measure the effect of
	 * injection on request service times, and then to update the
	 * limit accordingly.
	 *
	 * However, in the following special case, the inject limit is
	 * left to 1 even if the think time is short: bfqq's I/O is
	 * synchronized with that of some other queue, i.e., bfqq may
	 * receive new I/O only after the I/O of the other queue is
	 * completed. Keeping the inject limit to 1 allows the
	 * blocking I/O to be served while bfqq is in service. And
	 * this is very convenient both for bfqq and for overall
	 * throughput, as explained in detail in the comments in
	 * bfq_update_has_short_ttime().
	 *
	 * On the opposite end, if bfqq has a long think time, then
	 * start directly by 1, because:
	 * a) on the bright side, keeping at most one request in
	 * service in the drive is unlikely to cause any harm to the
	 * latency of bfqq's requests, as the service time of a single
	 * request is likely to be lower than the think time of bfqq;
	 * b) on the downside, after becoming empty, bfqq is likely to
	 * expire before getting its next request. With this request
	 * arrival pattern, it is very hard to sample total service
	 * times and update the inject limit accordingly (see comments
	 * on bfq_update_inject_limit()). So the limit is likely to be
	 * never, or at least seldom, updated.  As a consequence, by
	 * setting the limit to 1, we avoid that no injection ever
	 * occurs with bfqq. On the downside, this proactive step
	 * further reduces chances to actually compute the baseline
	 * total service time. Thus it reduces chances to execute the
	 * limit-update algorithm and possibly raise the limit to more
	 * than 1.
	 */
	if (bfq_bfqq_has_short_ttime(bfqq))
		bfqq->inject_limit = 0;
	else
		bfqq->inject_limit = 1;

	bfqq->decrease_time_jif = jiffies;