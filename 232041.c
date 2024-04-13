 */
static void bfq_prepare_request(struct request *rq, struct bio *bio)
{
	/*
	 * Regardless of whether we have an icq attached, we have to
	 * clear the scheduler pointers, as they might point to
	 * previously allocated bic/bfqq structs.
	 */
	rq->elv.priv[0] = rq->elv.priv[1] = NULL;