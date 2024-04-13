static int vhost_net_tx_get_vq_desc(struct vhost_net *net,
				    struct vhost_net_virtqueue *tnvq,
				    unsigned int *out_num, unsigned int *in_num,
				    struct msghdr *msghdr, bool *busyloop_intr)
{
	struct vhost_net_virtqueue *rnvq = &net->vqs[VHOST_NET_VQ_RX];
	struct vhost_virtqueue *rvq = &rnvq->vq;
	struct vhost_virtqueue *tvq = &tnvq->vq;

	int r = vhost_get_vq_desc(tvq, tvq->iov, ARRAY_SIZE(tvq->iov),
				  out_num, in_num, NULL, NULL);

	if (r == tvq->num && tvq->busyloop_timeout) {
		/* Flush batched packets first */
		if (!vhost_sock_zcopy(tvq->private_data))
			vhost_tx_batch(net, tnvq, tvq->private_data, msghdr);

		vhost_net_busy_poll(net, rvq, tvq, busyloop_intr, false);

		r = vhost_get_vq_desc(tvq, tvq->iov, ARRAY_SIZE(tvq->iov),
				      out_num, in_num, NULL, NULL);
	}

	return r;
}