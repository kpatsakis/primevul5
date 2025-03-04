static void handle_tx_zerocopy(struct vhost_net *net, struct socket *sock)
{
	struct vhost_net_virtqueue *nvq = &net->vqs[VHOST_NET_VQ_TX];
	struct vhost_virtqueue *vq = &nvq->vq;
	unsigned out, in;
	int head;
	struct msghdr msg = {
		.msg_name = NULL,
		.msg_namelen = 0,
		.msg_control = NULL,
		.msg_controllen = 0,
		.msg_flags = MSG_DONTWAIT,
	};
	struct tun_msg_ctl ctl;
	size_t len, total_len = 0;
	int err;
	struct vhost_net_ubuf_ref *uninitialized_var(ubufs);
	bool zcopy_used;
	int sent_pkts = 0;

	do {
		bool busyloop_intr;

		/* Release DMAs done buffers first */
		vhost_zerocopy_signal_used(net, vq);

		busyloop_intr = false;
		head = get_tx_bufs(net, nvq, &msg, &out, &in, &len,
				   &busyloop_intr);
		/* On error, stop handling until the next kick. */
		if (unlikely(head < 0))
			break;
		/* Nothing new?  Wait for eventfd to tell us they refilled. */
		if (head == vq->num) {
			if (unlikely(busyloop_intr)) {
				vhost_poll_queue(&vq->poll);
			} else if (unlikely(vhost_enable_notify(&net->dev, vq))) {
				vhost_disable_notify(&net->dev, vq);
				continue;
			}
			break;
		}

		zcopy_used = len >= VHOST_GOODCOPY_LEN
			     && !vhost_exceeds_maxpend(net)
			     && vhost_net_tx_select_zcopy(net);

		/* use msg_control to pass vhost zerocopy ubuf info to skb */
		if (zcopy_used) {
			struct ubuf_info *ubuf;
			ubuf = nvq->ubuf_info + nvq->upend_idx;

			vq->heads[nvq->upend_idx].id = cpu_to_vhost32(vq, head);
			vq->heads[nvq->upend_idx].len = VHOST_DMA_IN_PROGRESS;
			ubuf->callback = vhost_zerocopy_callback;
			ubuf->ctx = nvq->ubufs;
			ubuf->desc = nvq->upend_idx;
			refcount_set(&ubuf->refcnt, 1);
			msg.msg_control = &ctl;
			ctl.type = TUN_MSG_UBUF;
			ctl.ptr = ubuf;
			msg.msg_controllen = sizeof(ctl);
			ubufs = nvq->ubufs;
			atomic_inc(&ubufs->refcount);
			nvq->upend_idx = (nvq->upend_idx + 1) % UIO_MAXIOV;
		} else {
			msg.msg_control = NULL;
			ubufs = NULL;
		}
		total_len += len;
		if (tx_can_batch(vq, total_len) &&
		    likely(!vhost_exceeds_maxpend(net))) {
			msg.msg_flags |= MSG_MORE;
		} else {
			msg.msg_flags &= ~MSG_MORE;
		}

		/* TODO: Check specific error and bomb out unless ENOBUFS? */
		err = sock->ops->sendmsg(sock, &msg, len);
		if (unlikely(err < 0)) {
			if (zcopy_used) {
				vhost_net_ubuf_put(ubufs);
				nvq->upend_idx = ((unsigned)nvq->upend_idx - 1)
					% UIO_MAXIOV;
			}
			vhost_discard_vq_desc(vq, 1);
			vhost_net_enable_vq(net, vq);
			break;
		}
		if (err != len)
			pr_debug("Truncated TX packet: "
				 " len %d != %zd\n", err, len);
		if (!zcopy_used)
			vhost_add_used_and_signal(&net->dev, vq, head, 0);
		else
			vhost_zerocopy_signal_used(net, vq);
		vhost_net_tx_packet(net);
	} while (likely(!vhost_exceeds_weight(vq, ++sent_pkts, total_len)));
}