static void xdp_clear_umem_at_qid(struct net_device *dev, u16 queue_id)
{
	if (queue_id < dev->real_num_rx_queues)
		dev->_rx[queue_id].umem = NULL;
	if (queue_id < dev->real_num_tx_queues)
		dev->_tx[queue_id].umem = NULL;
}