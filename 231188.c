static int get_tx_bufs(struct vhost_net *net,
		       struct vhost_net_virtqueue *nvq,
		       struct msghdr *msg,
		       unsigned int *out, unsigned int *in,
		       size_t *len, bool *busyloop_intr)
{
	struct vhost_virtqueue *vq = &nvq->vq;
	int ret;

	ret = vhost_net_tx_get_vq_desc(net, nvq, out, in, msg, busyloop_intr);

	if (ret < 0 || ret == vq->num)
		return ret;

	if (*in) {
		vq_err(vq, "Unexpected descriptor format for TX: out %d, int %d\n",
			*out, *in);
		return -EFAULT;
	}

	/* Sanity check */
	*len = init_iov_iter(vq, &msg->msg_iter, nvq->vhost_hlen, *out);
	if (*len == 0) {
		vq_err(vq, "Unexpected header len for TX: %zd expected %zd\n",
			*len, nvq->vhost_hlen);
		return -EFAULT;
	}

	return ret;
}