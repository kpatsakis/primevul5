mt76_dma_sync_idx(struct mt76_dev *dev, struct mt76_queue *q)
{
	writel(q->desc_dma, &q->regs->desc_base);
	writel(q->ndesc, &q->regs->ring_size);
	q->head = readl(&q->regs->dma_idx);
	q->tail = q->head;
	writel(q->head, &q->regs->cpu_idx);
}