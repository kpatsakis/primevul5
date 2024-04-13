mt76_dma_kick_queue(struct mt76_dev *dev, struct mt76_queue *q)
{
	writel(q->head, &q->regs->cpu_idx);
}