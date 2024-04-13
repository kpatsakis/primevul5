
static struct request *bfq_dispatch_request(struct blk_mq_hw_ctx *hctx)
{
	struct bfq_data *bfqd = hctx->queue->elevator->elevator_data;
	struct request *rq;
	struct bfq_queue *in_serv_queue;
	bool waiting_rq, idle_timer_disabled;

	spin_lock_irq(&bfqd->lock);

	in_serv_queue = bfqd->in_service_queue;
	waiting_rq = in_serv_queue && bfq_bfqq_wait_request(in_serv_queue);

	rq = __bfq_dispatch_request(hctx);

	idle_timer_disabled =
		waiting_rq && !bfq_bfqq_wait_request(in_serv_queue);

	spin_unlock_irq(&bfqd->lock);

	bfq_update_dispatch_stats(hctx->queue, rq, in_serv_queue,
				  idle_timer_disabled);

	return rq;