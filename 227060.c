reserve_avail_buf_split(struct virtio_net *dev, struct vhost_virtqueue *vq,
				uint32_t size, struct buf_vector *buf_vec,
				uint16_t *num_buffers, uint16_t avail_head,
				uint16_t *nr_vec)
{
	uint16_t cur_idx;
	uint16_t vec_idx = 0;
	uint16_t max_tries, tries = 0;

	uint16_t head_idx = 0;
	uint32_t len = 0;

	*num_buffers = 0;
	cur_idx  = vq->last_avail_idx;

	if (rxvq_is_mergeable(dev))
		max_tries = vq->size - 1;
	else
		max_tries = 1;

	while (size > 0) {
		if (unlikely(cur_idx == avail_head))
			return -1;
		/*
		 * if we tried all available ring items, and still
		 * can't get enough buf, it means something abnormal
		 * happened.
		 */
		if (unlikely(++tries > max_tries))
			return -1;

		if (unlikely(fill_vec_buf_split(dev, vq, cur_idx,
						&vec_idx, buf_vec,
						&head_idx, &len,
						VHOST_ACCESS_RW) < 0))
			return -1;
		len = RTE_MIN(len, size);
		update_shadow_used_ring_split(vq, head_idx, len);
		size -= len;

		cur_idx++;
		*num_buffers += 1;
	}

	*nr_vec = vec_idx;

	return 0;
}