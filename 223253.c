struct xdp_umem *xdp_get_umem_from_qid(struct net_device *dev,
				       u16 queue_id)
{
	if (queue_id < dev->real_num_rx_queues)
		return dev->_rx[queue_id].umem;
	if (queue_id < dev->real_num_tx_queues)
		return dev->_tx[queue_id].umem;

	return NULL;
}