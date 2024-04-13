static void rx_refill_timeout(struct timer_list *t)
{
	struct netfront_queue *queue = from_timer(queue, t, rx_refill_timer);
	napi_schedule(&queue->napi);
}