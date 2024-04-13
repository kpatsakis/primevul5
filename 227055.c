fill_vec_buf_packed(struct virtio_net *dev, struct vhost_virtqueue *vq,
				uint16_t avail_idx, uint16_t *desc_count,
				struct buf_vector *buf_vec, uint16_t *vec_idx,
				uint16_t *buf_id, uint32_t *len, uint8_t perm)
{
	bool wrap_counter = vq->avail_wrap_counter;
	struct vring_packed_desc *descs = vq->desc_packed;
	uint16_t vec_id = *vec_idx;

	if (avail_idx < vq->last_avail_idx)
		wrap_counter ^= 1;

	/*
	 * Perform a load-acquire barrier in desc_is_avail to
	 * enforce the ordering between desc flags and desc
	 * content.
	 */
	if (unlikely(!desc_is_avail(&descs[avail_idx], wrap_counter)))
		return -1;

	*desc_count = 0;
	*len = 0;

	while (1) {
		if (unlikely(vec_id >= BUF_VECTOR_MAX))
			return -1;

		if (unlikely(*desc_count >= vq->size))
			return -1;

		*desc_count += 1;
		*buf_id = descs[avail_idx].id;

		if (descs[avail_idx].flags & VRING_DESC_F_INDIRECT) {
			if (unlikely(fill_vec_buf_packed_indirect(dev, vq,
							&descs[avail_idx],
							&vec_id, buf_vec,
							len, perm) < 0))
				return -1;
		} else {
			*len += descs[avail_idx].len;

			if (unlikely(map_one_desc(dev, vq, buf_vec, &vec_id,
							descs[avail_idx].addr,
							descs[avail_idx].len,
							perm)))
				return -1;
		}

		if ((descs[avail_idx].flags & VRING_DESC_F_NEXT) == 0)
			break;

		if (++avail_idx >= vq->size) {
			avail_idx -= vq->size;
			wrap_counter ^= 1;
		}
	}

	*vec_idx = vec_id;

	return 0;
}