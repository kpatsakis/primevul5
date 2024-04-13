void mt76_dma_cleanup(struct mt76_dev *dev)
{
	int i;

	netif_napi_del(&dev->tx_napi);
	for (i = 0; i < ARRAY_SIZE(dev->q_tx); i++)
		mt76_dma_tx_cleanup(dev, i, true);

	for (i = 0; i < ARRAY_SIZE(dev->q_rx); i++) {
		netif_napi_del(&dev->napi[i]);
		mt76_dma_rx_cleanup(dev, &dev->q_rx[i]);
	}
}