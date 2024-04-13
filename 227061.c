vhost_shadow_enqueue_single_packed(struct virtio_net *dev,
				   struct vhost_virtqueue *vq,
				   uint32_t len[],
				   uint16_t id[],
				   uint16_t count[],
				   uint16_t num_buffers)
{
	uint16_t i;
	for (i = 0; i < num_buffers; i++) {
		/* enqueue shadow flush action aligned with batch num */
		if (!vq->shadow_used_idx)
			vq->shadow_aligned_idx = vq->last_used_idx &
				PACKED_BATCH_MASK;
		vq->shadow_used_packed[vq->shadow_used_idx].id  = id[i];
		vq->shadow_used_packed[vq->shadow_used_idx].len = len[i];
		vq->shadow_used_packed[vq->shadow_used_idx].count = count[i];
		vq->shadow_aligned_idx += count[i];
		vq->shadow_used_idx++;
	}

	if (vq->shadow_aligned_idx >= PACKED_BATCH_SIZE) {
		do_data_copy_enqueue(dev, vq);
		vhost_flush_enqueue_shadow_packed(dev, vq);
	}
}