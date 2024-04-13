
struct bfq_queue *bic_to_bfqq(struct bfq_io_cq *bic, bool is_sync)
{
	return bic->bfqq[is_sync];