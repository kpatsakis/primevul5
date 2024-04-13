mt76_dma_init(struct mt76_dev *dev)
{
	int i;

	init_dummy_netdev(&dev->napi_dev);

	for (i = 0; i < ARRAY_SIZE(dev->q_rx); i++) {
		netif_napi_add(&dev->napi_dev, &dev->napi[i], mt76_dma_rx_poll,
			       64);
		mt76_dma_rx_fill(dev, &dev->q_rx[i]);
		skb_queue_head_init(&dev->rx_skb[i]);
		napi_enable(&dev->napi[i]);
	}

	return 0;
}