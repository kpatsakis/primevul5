mt76_dma_tx_cleanup_idx(struct mt76_dev *dev, struct mt76_queue *q, int idx,
			struct mt76_queue_entry *prev_e)
{
	struct mt76_queue_entry *e = &q->entry[idx];
	__le32 __ctrl = READ_ONCE(q->desc[idx].ctrl);
	u32 ctrl = le32_to_cpu(__ctrl);

	if (!e->skip_buf0) {
		__le32 addr = READ_ONCE(q->desc[idx].buf0);
		u32 len = FIELD_GET(MT_DMA_CTL_SD_LEN0, ctrl);

		dma_unmap_single(dev->dev, le32_to_cpu(addr), len,
				 DMA_TO_DEVICE);
	}

	if (!(ctrl & MT_DMA_CTL_LAST_SEC0)) {
		__le32 addr = READ_ONCE(q->desc[idx].buf1);
		u32 len = FIELD_GET(MT_DMA_CTL_SD_LEN1, ctrl);

		dma_unmap_single(dev->dev, le32_to_cpu(addr), len,
				 DMA_TO_DEVICE);
	}

	if (e->txwi == DMA_DUMMY_DATA)
		e->txwi = NULL;

	if (e->skb == DMA_DUMMY_DATA)
		e->skb = NULL;

	*prev_e = *e;
	memset(e, 0, sizeof(*e));
}