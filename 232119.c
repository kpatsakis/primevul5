/* see the definition of bfq_async_charge_factor for details */
static unsigned long bfq_serv_to_charge(struct request *rq,
					struct bfq_queue *bfqq)
{
	if (bfq_bfqq_sync(bfqq) || bfqq->wr_coeff > 1 ||
	    bfq_asymmetric_scenario(bfqq->bfqd, bfqq))
		return blk_rq_sectors(rq);

	return blk_rq_sectors(rq) * bfq_async_charge_factor;