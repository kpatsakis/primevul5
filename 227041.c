vhost_shadow_dequeue_batch_packed_inorder(struct vhost_virtqueue *vq,
					  uint16_t id)
{
	vq->shadow_used_packed[0].id = id;

	if (!vq->shadow_used_idx) {
		vq->shadow_last_used_idx = vq->last_used_idx;
		vq->shadow_used_packed[0].flags =
			PACKED_DESC_DEQUEUE_USED_FLAG(vq->used_wrap_counter);
		vq->shadow_used_packed[0].len = 0;
		vq->shadow_used_packed[0].count = 1;
		vq->shadow_used_idx++;
	}

	vq_inc_last_used_packed(vq, PACKED_BATCH_SIZE);
}