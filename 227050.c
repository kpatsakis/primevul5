do_flush_shadow_used_ring_split(struct virtio_net *dev,
			struct vhost_virtqueue *vq,
			uint16_t to, uint16_t from, uint16_t size)
{
	rte_memcpy(&vq->used->ring[to],
			&vq->shadow_used_split[from],
			size * sizeof(struct vring_used_elem));
	vhost_log_cache_used_vring(dev, vq,
			offsetof(struct vring_used, ring[to]),
			size * sizeof(struct vring_used_elem));
}