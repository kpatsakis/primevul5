vhost_enqueue_single_packed(struct virtio_net *dev,
			    struct vhost_virtqueue *vq,
			    struct rte_mbuf *pkt,
			    struct buf_vector *buf_vec,
			    uint16_t *nr_descs)
{
	uint16_t nr_vec = 0;
	uint16_t avail_idx = vq->last_avail_idx;
	uint16_t max_tries, tries = 0;
	uint16_t buf_id = 0;
	uint32_t len = 0;
	uint16_t desc_count;
	uint32_t size = pkt->pkt_len + dev->vhost_hlen;
	uint16_t num_buffers = 0;
	uint32_t buffer_len[vq->size];
	uint16_t buffer_buf_id[vq->size];
	uint16_t buffer_desc_count[vq->size];

	if (rxvq_is_mergeable(dev))
		max_tries = vq->size - 1;
	else
		max_tries = 1;

	while (size > 0) {
		/*
		 * if we tried all available ring items, and still
		 * can't get enough buf, it means something abnormal
		 * happened.
		 */
		if (unlikely(++tries > max_tries))
			return -1;

		if (unlikely(fill_vec_buf_packed(dev, vq,
						avail_idx, &desc_count,
						buf_vec, &nr_vec,
						&buf_id, &len,
						VHOST_ACCESS_RW) < 0))
			return -1;

		len = RTE_MIN(len, size);
		size -= len;

		buffer_len[num_buffers] = len;
		buffer_buf_id[num_buffers] = buf_id;
		buffer_desc_count[num_buffers] = desc_count;
		num_buffers += 1;

		*nr_descs += desc_count;
		avail_idx += desc_count;
		if (avail_idx >= vq->size)
			avail_idx -= vq->size;
	}

	if (copy_mbuf_to_desc(dev, vq, pkt, buf_vec, nr_vec, num_buffers) < 0)
		return -1;

	vhost_shadow_enqueue_single_packed(dev, vq, buffer_len, buffer_buf_id,
					   buffer_desc_count, num_buffers);

	return 0;
}