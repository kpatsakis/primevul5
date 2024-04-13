mt76_dma_tx_cleanup(struct mt76_dev *dev, enum mt76_txq_id qid, bool flush)
{
	struct mt76_sw_queue *sq = &dev->q_tx[qid];
	struct mt76_queue *q = sq->q;
	struct mt76_queue_entry entry;
	unsigned int n_swq_queued[4] = {};
	unsigned int n_queued = 0;
	bool wake = false;
	int i, last;

	if (!q)
		return;

	if (flush)
		last = -1;
	else
		last = readl(&q->regs->dma_idx);

	while ((q->queued > n_queued) && q->tail != last) {
		mt76_dma_tx_cleanup_idx(dev, q, q->tail, &entry);
		if (entry.schedule)
			n_swq_queued[entry.qid]++;

		q->tail = (q->tail + 1) % q->ndesc;
		n_queued++;

		if (entry.skb)
			dev->drv->tx_complete_skb(dev, qid, &entry);

		if (entry.txwi) {
			if (!(dev->drv->drv_flags & MT_DRV_TXWI_NO_FREE))
				mt76_put_txwi(dev, entry.txwi);
			wake = !flush;
		}

		if (!flush && q->tail == last)
			last = readl(&q->regs->dma_idx);
	}

	spin_lock_bh(&q->lock);

	q->queued -= n_queued;
	for (i = 0; i < ARRAY_SIZE(n_swq_queued); i++) {
		if (!n_swq_queued[i])
			continue;

		dev->q_tx[i].swq_queued -= n_swq_queued[i];
	}

	if (flush)
		mt76_dma_sync_idx(dev, q);

	wake = wake && q->stopped &&
	       qid < IEEE80211_NUM_ACS && q->queued < q->ndesc - 8;
	if (wake)
		q->stopped = false;

	if (!q->queued)
		wake_up(&dev->tx_wait);

	spin_unlock_bh(&q->lock);

	if (wake)
		ieee80211_wake_queue(dev->hw, qid);
}