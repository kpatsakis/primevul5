static void xennet_set_rx_rsp_cons(struct netfront_queue *queue, RING_IDX val)
{
	unsigned long flags;

	spin_lock_irqsave(&queue->rx_cons_lock, flags);
	queue->rx.rsp_cons = val;
	queue->rx_rsp_unconsumed = XEN_RING_NR_UNCONSUMED_RESPONSES(&queue->rx);
	spin_unlock_irqrestore(&queue->rx_cons_lock, flags);
}