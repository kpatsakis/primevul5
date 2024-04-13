static int vhost_net_rx_peek_head_len(struct vhost_net *net, struct sock *sk,
				      bool *busyloop_intr)
{
	struct vhost_net_virtqueue *rnvq = &net->vqs[VHOST_NET_VQ_RX];
	struct vhost_net_virtqueue *tnvq = &net->vqs[VHOST_NET_VQ_TX];
	struct vhost_virtqueue *rvq = &rnvq->vq;
	struct vhost_virtqueue *tvq = &tnvq->vq;
	int len = peek_head_len(rnvq, sk);

	if (!len && rvq->busyloop_timeout) {
		/* Flush batched heads first */
		vhost_net_signal_used(rnvq);
		/* Both tx vq and rx socket were polled here */
		vhost_net_busy_poll(net, rvq, tvq, busyloop_intr, true);

		len = peek_head_len(rnvq, sk);
	}

	return len;
}