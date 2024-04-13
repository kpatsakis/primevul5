static int qrtr_local_enqueue(struct qrtr_node *node, struct sk_buff *skb,
			      int type, struct sockaddr_qrtr *from,
			      struct sockaddr_qrtr *to)
{
	struct qrtr_sock *ipc;
	struct qrtr_cb *cb;

	ipc = qrtr_port_lookup(to->sq_port);
	if (!ipc || &ipc->sk == skb->sk) { /* do not send to self */
		kfree_skb(skb);
		return -ENODEV;
	}

	cb = (struct qrtr_cb *)skb->cb;
	cb->src_node = from->sq_node;
	cb->src_port = from->sq_port;

	if (sock_queue_rcv_skb(&ipc->sk, skb)) {
		qrtr_port_put(ipc);
		kfree_skb(skb);
		return -ENOSPC;
	}

	qrtr_port_put(ipc);

	return 0;
}