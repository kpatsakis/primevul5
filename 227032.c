vhost_flush_enqueue_shadow_packed(struct virtio_net *dev,
				  struct vhost_virtqueue *vq)
{
	int i;
	uint16_t used_idx = vq->last_used_idx;
	uint16_t head_idx = vq->last_used_idx;
	uint16_t head_flags = 0;

	/* Split loop in two to save memory barriers */
	for (i = 0; i < vq->shadow_used_idx; i++) {
		vq->desc_packed[used_idx].id = vq->shadow_used_packed[i].id;
		vq->desc_packed[used_idx].len = vq->shadow_used_packed[i].len;

		used_idx += vq->shadow_used_packed[i].count;
		if (used_idx >= vq->size)
			used_idx -= vq->size;
	}

	rte_smp_wmb();

	for (i = 0; i < vq->shadow_used_idx; i++) {
		uint16_t flags;

		if (vq->shadow_used_packed[i].len)
			flags = VRING_DESC_F_WRITE;
		else
			flags = 0;

		if (vq->used_wrap_counter) {
			flags |= VRING_DESC_F_USED;
			flags |= VRING_DESC_F_AVAIL;
		} else {
			flags &= ~VRING_DESC_F_USED;
			flags &= ~VRING_DESC_F_AVAIL;
		}

		if (i > 0) {
			vq->desc_packed[vq->last_used_idx].flags = flags;

			vhost_log_cache_used_vring(dev, vq,
					vq->last_used_idx *
					sizeof(struct vring_packed_desc),
					sizeof(struct vring_packed_desc));
		} else {
			head_idx = vq->last_used_idx;
			head_flags = flags;
		}

		vq_inc_last_used_packed(vq, vq->shadow_used_packed[i].count);
	}

	vq->desc_packed[head_idx].flags = head_flags;

	vhost_log_cache_used_vring(dev, vq,
				head_idx *
				sizeof(struct vring_packed_desc),
				sizeof(struct vring_packed_desc));

	vq->shadow_used_idx = 0;
	vhost_log_cache_sync(dev, vq);
}