static void qrtr_port_remove(struct qrtr_sock *ipc)
{
	struct qrtr_ctrl_pkt *pkt;
	struct sk_buff *skb;
	int port = ipc->us.sq_port;
	struct sockaddr_qrtr to;

	to.sq_family = AF_QIPCRTR;
	to.sq_node = QRTR_NODE_BCAST;
	to.sq_port = QRTR_PORT_CTRL;

	skb = qrtr_alloc_ctrl_packet(&pkt, GFP_KERNEL);
	if (skb) {
		pkt->cmd = cpu_to_le32(QRTR_TYPE_DEL_CLIENT);
		pkt->client.node = cpu_to_le32(ipc->us.sq_node);
		pkt->client.port = cpu_to_le32(ipc->us.sq_port);

		skb_set_owner_w(skb, &ipc->sk);
		qrtr_bcast_enqueue(NULL, skb, QRTR_TYPE_DEL_CLIENT, &ipc->us,
				   &to);
	}

	if (port == QRTR_PORT_CTRL)
		port = 0;

	__sock_put(&ipc->sk);

	mutex_lock(&qrtr_port_lock);
	idr_remove(&qrtr_ports, port);
	mutex_unlock(&qrtr_port_lock);

	/* Ensure that if qrtr_port_lookup() did enter the RCU read section we
	 * wait for it to up increment the refcount */
	synchronize_rcu();
}