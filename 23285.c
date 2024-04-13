static bool xennet_handle_tx(struct netfront_queue *queue, unsigned int *eoi)
{
	unsigned long flags;

	if (unlikely(queue->info->broken))
		return false;

	spin_lock_irqsave(&queue->tx_lock, flags);
	if (xennet_tx_buf_gc(queue))
		*eoi = 0;
	spin_unlock_irqrestore(&queue->tx_lock, flags);

	return true;
}