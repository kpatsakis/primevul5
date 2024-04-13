vhost_flush_enqueue_batch_packed(struct virtio_net *dev,
				 struct vhost_virtqueue *vq,
				 uint64_t *lens,
				 uint16_t *ids)
{
	uint16_t i;
	uint16_t flags;

	if (vq->shadow_used_idx) {
		do_data_copy_enqueue(dev, vq);
		vhost_flush_enqueue_shadow_packed(dev, vq);
	}

	flags = PACKED_DESC_ENQUEUE_USED_FLAG(vq->used_wrap_counter);

	vhost_for_each_try_unroll(i, 0, PACKED_BATCH_SIZE) {
		vq->desc_packed[vq->last_used_idx + i].id = ids[i];
		vq->desc_packed[vq->last_used_idx + i].len = lens[i];
	}

	rte_smp_wmb();

	vhost_for_each_try_unroll(i, 0, PACKED_BATCH_SIZE)
		vq->desc_packed[vq->last_used_idx + i].flags = flags;

	vhost_log_cache_used_vring(dev, vq, vq->last_used_idx *
				   sizeof(struct vring_packed_desc),
				   sizeof(struct vring_packed_desc) *
				   PACKED_BATCH_SIZE);
	vhost_log_cache_sync(dev, vq);

	vq_inc_last_used_packed(vq, PACKED_BATCH_SIZE);
}