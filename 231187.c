static void vhost_net_signal_used(struct vhost_net_virtqueue *nvq)
{
	struct vhost_virtqueue *vq = &nvq->vq;
	struct vhost_dev *dev = vq->dev;

	if (!nvq->done_idx)
		return;

	vhost_add_used_and_signal_n(dev, vq, vq->heads, nvq->done_idx);
	nvq->done_idx = 0;
}