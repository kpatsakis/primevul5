vhost_flush_dequeue_shadow_packed(struct virtio_net *dev,
				  struct vhost_virtqueue *vq)
{
	struct vring_used_elem_packed *used_elem = &vq->shadow_used_packed[0];

	vq->desc_packed[vq->shadow_last_used_idx].id = used_elem->id;
	rte_smp_wmb();
	vq->desc_packed[vq->shadow_last_used_idx].flags = used_elem->flags;

	vhost_log_cache_used_vring(dev, vq, vq->shadow_last_used_idx *
				   sizeof(struct vring_packed_desc),
				   sizeof(struct vring_packed_desc));
	vq->shadow_used_idx = 0;
	vhost_log_cache_sync(dev, vq);
}