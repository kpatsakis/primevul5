virtio_dev_tx_split(struct virtio_net *dev, struct vhost_virtqueue *vq,
	struct rte_mempool *mbuf_pool, struct rte_mbuf **pkts, uint16_t count)
{
	uint16_t i;
	uint16_t free_entries;

	if (unlikely(dev->dequeue_zero_copy)) {
		struct zcopy_mbuf *zmbuf, *next;

		for (zmbuf = TAILQ_FIRST(&vq->zmbuf_list);
		     zmbuf != NULL; zmbuf = next) {
			next = TAILQ_NEXT(zmbuf, next);

			if (mbuf_is_consumed(zmbuf->mbuf)) {
				update_shadow_used_ring_split(vq,
						zmbuf->desc_idx, 0);
				TAILQ_REMOVE(&vq->zmbuf_list, zmbuf, next);
				restore_mbuf(zmbuf->mbuf);
				rte_pktmbuf_free(zmbuf->mbuf);
				put_zmbuf(zmbuf);
				vq->nr_zmbuf -= 1;
			}
		}

		if (likely(vq->shadow_used_idx)) {
			flush_shadow_used_ring_split(dev, vq);
			vhost_vring_call_split(dev, vq);
		}
	}

	/*
	 * The ordering between avail index and
	 * desc reads needs to be enforced.
	 */
	free_entries = __atomic_load_n(&vq->avail->idx, __ATOMIC_ACQUIRE) -
			vq->last_avail_idx;
	if (free_entries == 0)
		return 0;

	rte_prefetch0(&vq->avail->ring[vq->last_avail_idx & (vq->size - 1)]);

	VHOST_LOG_DATA(DEBUG, "(%d) %s\n", dev->vid, __func__);

	count = RTE_MIN(count, MAX_PKT_BURST);
	count = RTE_MIN(count, free_entries);
	VHOST_LOG_DATA(DEBUG, "(%d) about to dequeue %u buffers\n",
			dev->vid, count);

	for (i = 0; i < count; i++) {
		struct buf_vector buf_vec[BUF_VECTOR_MAX];
		uint16_t head_idx;
		uint32_t buf_len;
		uint16_t nr_vec = 0;
		int err;

		if (unlikely(fill_vec_buf_split(dev, vq,
						vq->last_avail_idx + i,
						&nr_vec, buf_vec,
						&head_idx, &buf_len,
						VHOST_ACCESS_RO) < 0))
			break;

		if (likely(dev->dequeue_zero_copy == 0))
			update_shadow_used_ring_split(vq, head_idx, 0);

		pkts[i] = virtio_dev_pktmbuf_alloc(dev, mbuf_pool, buf_len);
		if (unlikely(pkts[i] == NULL))
			break;

		err = copy_desc_to_mbuf(dev, vq, buf_vec, nr_vec, pkts[i],
				mbuf_pool);
		if (unlikely(err)) {
			rte_pktmbuf_free(pkts[i]);
			break;
		}

		if (unlikely(dev->dequeue_zero_copy)) {
			struct zcopy_mbuf *zmbuf;

			zmbuf = get_zmbuf(vq);
			if (!zmbuf) {
				rte_pktmbuf_free(pkts[i]);
				break;
			}
			zmbuf->mbuf = pkts[i];
			zmbuf->desc_idx = head_idx;

			/*
			 * Pin lock the mbuf; we will check later to see
			 * whether the mbuf is freed (when we are the last
			 * user) or not. If that's the case, we then could
			 * update the used ring safely.
			 */
			rte_mbuf_refcnt_update(pkts[i], 1);

			vq->nr_zmbuf += 1;
			TAILQ_INSERT_TAIL(&vq->zmbuf_list, zmbuf, next);
		}
	}
	vq->last_avail_idx += i;

	if (likely(dev->dequeue_zero_copy == 0)) {
		do_data_copy_dequeue(vq);
		if (unlikely(i < count))
			vq->shadow_used_idx = i;
		if (likely(vq->shadow_used_idx)) {
			flush_shadow_used_ring_split(dev, vq);
			vhost_vring_call_split(dev, vq);
		}
	}

	return i;
}