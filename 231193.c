static void vhost_tx_batch(struct vhost_net *net,
			   struct vhost_net_virtqueue *nvq,
			   struct socket *sock,
			   struct msghdr *msghdr)
{
	struct tun_msg_ctl ctl = {
		.type = TUN_MSG_PTR,
		.num = nvq->batched_xdp,
		.ptr = nvq->xdp,
	};
	int err;

	if (nvq->batched_xdp == 0)
		goto signal_used;

	msghdr->msg_control = &ctl;
	err = sock->ops->sendmsg(sock, msghdr, 0);
	if (unlikely(err < 0)) {
		vq_err(&nvq->vq, "Fail to batch sending packets\n");
		return;
	}

signal_used:
	vhost_net_signal_used(nvq);
	nvq->batched_xdp = 0;
}