 */
static bool bfq_bfqq_higher_class_or_weight(struct bfq_queue *bfqq,
					    struct bfq_queue *in_serv_bfqq)
{
	int bfqq_weight, in_serv_weight;

	if (bfqq->ioprio_class < in_serv_bfqq->ioprio_class)
		return true;

	if (in_serv_bfqq->entity.parent == bfqq->entity.parent) {
		bfqq_weight = bfqq->entity.weight;
		in_serv_weight = in_serv_bfqq->entity.weight;
	} else {
		if (bfqq->entity.parent)
			bfqq_weight = bfqq->entity.parent->weight;
		else
			bfqq_weight = bfqq->entity.weight;
		if (in_serv_bfqq->entity.parent)
			in_serv_weight = in_serv_bfqq->entity.parent->weight;
		else
			in_serv_weight = in_serv_bfqq->entity.weight;
	}

	return bfqq_weight > in_serv_weight;