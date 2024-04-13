static int xdp_reg_umem_at_qid(struct net_device *dev, struct xdp_umem *umem,
			       u16 queue_id)
{
	if (queue_id >= max_t(unsigned int,
			      dev->real_num_rx_queues,
			      dev->real_num_tx_queues))
		return -EINVAL;

	if (queue_id < dev->real_num_rx_queues)
		dev->_rx[queue_id].umem = umem;
	if (queue_id < dev->real_num_tx_queues)
		dev->_tx[queue_id].umem = umem;

	return 0;
}