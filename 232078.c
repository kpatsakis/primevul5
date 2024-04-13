static void
bfq_update_io_seektime(struct bfq_data *bfqd, struct bfq_queue *bfqq,
		       struct request *rq)
{
	bfqq->seek_history <<= 1;
	bfqq->seek_history |= BFQ_RQ_SEEKY(bfqd, bfqq->last_request_pos, rq);

	if (bfqq->wr_coeff > 1 &&
	    bfqq->wr_cur_max_time == bfqd->bfq_wr_rt_max_time &&
	    BFQQ_TOTALLY_SEEKY(bfqq))
		bfq_bfqq_end_wr(bfqq);