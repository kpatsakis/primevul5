static int qrtr_tx_wait(struct qrtr_node *node, int dest_node, int dest_port,
			int type)
{
	unsigned long key = (u64)dest_node << 32 | dest_port;
	struct qrtr_tx_flow *flow;
	int confirm_rx = 0;
	int ret;

	/* Never set confirm_rx on non-data packets */
	if (type != QRTR_TYPE_DATA)
		return 0;

	mutex_lock(&node->qrtr_tx_lock);
	flow = radix_tree_lookup(&node->qrtr_tx_flow, key);
	if (!flow) {
		flow = kzalloc(sizeof(*flow), GFP_KERNEL);
		if (flow) {
			init_waitqueue_head(&flow->resume_tx);
			radix_tree_insert(&node->qrtr_tx_flow, key, flow);
		}
	}
	mutex_unlock(&node->qrtr_tx_lock);

	/* Set confirm_rx if we where unable to find and allocate a flow */
	if (!flow)
		return 1;

	spin_lock_irq(&flow->resume_tx.lock);
	ret = wait_event_interruptible_locked_irq(flow->resume_tx,
						  flow->pending < QRTR_TX_FLOW_HIGH ||
						  flow->tx_failed ||
						  !node->ep);
	if (ret < 0) {
		confirm_rx = ret;
	} else if (!node->ep) {
		confirm_rx = -EPIPE;
	} else if (flow->tx_failed) {
		flow->tx_failed = 0;
		confirm_rx = 1;
	} else {
		flow->pending++;
		confirm_rx = flow->pending == QRTR_TX_FLOW_LOW;
	}
	spin_unlock_irq(&flow->resume_tx.lock);

	return confirm_rx;
}