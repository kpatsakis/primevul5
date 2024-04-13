
void bic_set_bfqq(struct bfq_io_cq *bic, struct bfq_queue *bfqq, bool is_sync)
{
	bic->bfqq[is_sync] = bfqq;