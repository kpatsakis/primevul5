vhost_shadow_dequeue_single_packed(struct vhost_virtqueue *vq,
				   uint16_t buf_id,
				   uint16_t count)
{
	uint16_t flags;

	flags = vq->desc_packed[vq->last_used_idx].flags;
	if (vq->used_wrap_counter) {
		flags |= VRING_DESC_F_USED;
		flags |= VRING_DESC_F_AVAIL;
	} else {
		flags &= ~VRING_DESC_F_USED;
		flags &= ~VRING_DESC_F_AVAIL;
	}

	if (!vq->shadow_used_idx) {
		vq->shadow_last_used_idx = vq->last_used_idx;

		vq->shadow_used_packed[0].id  = buf_id;
		vq->shadow_used_packed[0].len = 0;
		vq->shadow_used_packed[0].flags = flags;
		vq->shadow_used_idx++;
	} else {
		vq->desc_packed[vq->last_used_idx].id = buf_id;
		vq->desc_packed[vq->last_used_idx].len = 0;
		vq->desc_packed[vq->last_used_idx].flags = flags;
	}

	vq_inc_last_used_packed(vq, count);
}