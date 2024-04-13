vhost_shadow_dequeue_batch_packed(struct virtio_net *dev,
				  struct vhost_virtqueue *vq,
				  uint16_t *ids)
{
	uint16_t flags;
	uint16_t i;
	uint16_t begin;

	flags = PACKED_DESC_DEQUEUE_USED_FLAG(vq->used_wrap_counter);

	if (!vq->shadow_used_idx) {
		vq->shadow_last_used_idx = vq->last_used_idx;
		vq->shadow_used_packed[0].id  = ids[0];
		vq->shadow_used_packed[0].len = 0;
		vq->shadow_used_packed[0].count = 1;
		vq->shadow_used_packed[0].flags = flags;
		vq->shadow_used_idx++;
		begin = 1;
	} else
		begin = 0;

	vhost_for_each_try_unroll(i, begin, PACKED_BATCH_SIZE) {
		vq->desc_packed[vq->last_used_idx + i].id = ids[i];
		vq->desc_packed[vq->last_used_idx + i].len = 0;
	}

	rte_smp_wmb();
	vhost_for_each_try_unroll(i, begin, PACKED_BATCH_SIZE)
		vq->desc_packed[vq->last_used_idx + i].flags = flags;

	vhost_log_cache_used_vring(dev, vq, vq->last_used_idx *
				   sizeof(struct vring_packed_desc),
				   sizeof(struct vring_packed_desc) *
				   PACKED_BATCH_SIZE);
	vhost_log_cache_sync(dev, vq);

	vq_inc_last_used_packed(vq, PACKED_BATCH_SIZE);
}