static bool tx_can_batch(struct vhost_virtqueue *vq, size_t total_len)
{
	return total_len < VHOST_NET_WEIGHT &&
	       !vhost_vq_avail_empty(vq->dev, vq);
}