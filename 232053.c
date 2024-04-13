
static int bfqq_process_refs(struct bfq_queue *bfqq)
{
	return bfqq->ref - bfqq->allocated - bfqq->entity.on_st_or_in_serv -
		(bfqq->weight_counter != NULL);