vhost_dequeue_single_packed(struct virtio_net *dev,
			    struct vhost_virtqueue *vq,
			    struct rte_mempool *mbuf_pool,
			    struct rte_mbuf **pkts,
			    uint16_t *buf_id,
			    uint16_t *desc_count)
{
	struct buf_vector buf_vec[BUF_VECTOR_MAX];
	uint32_t buf_len;
	uint16_t nr_vec = 0;
	int err;

	if (unlikely(fill_vec_buf_packed(dev, vq,
					 vq->last_avail_idx, desc_count,
					 buf_vec, &nr_vec,
					 buf_id, &buf_len,
					 VHOST_ACCESS_RO) < 0))
		return -1;

	*pkts = virtio_dev_pktmbuf_alloc(dev, mbuf_pool, buf_len);
	if (unlikely(*pkts == NULL)) {
		VHOST_LOG_DATA(ERR,
			"Failed to allocate memory for mbuf.\n");
		return -1;
	}

	err = copy_desc_to_mbuf(dev, vq, buf_vec, nr_vec, *pkts,
				mbuf_pool);
	if (unlikely(err)) {
		rte_pktmbuf_free(*pkts);
		return -1;
	}

	return 0;
}