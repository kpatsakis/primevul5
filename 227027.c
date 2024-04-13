virtio_dev_tx_batch_packed_zmbuf(struct virtio_net *dev,
				 struct vhost_virtqueue *vq,
				 struct rte_mempool *mbuf_pool,
				 struct rte_mbuf **pkts)
{
	struct zcopy_mbuf *zmbufs[PACKED_BATCH_SIZE];
	uintptr_t desc_addrs[PACKED_BATCH_SIZE];
	uint16_t ids[PACKED_BATCH_SIZE];
	uint16_t i;

	uint16_t avail_idx = vq->last_avail_idx;

	if (vhost_reserve_avail_batch_packed(dev, vq, mbuf_pool, pkts,
					     avail_idx, desc_addrs, ids))
		return -1;

	vhost_for_each_try_unroll(i, 0, PACKED_BATCH_SIZE)
		zmbufs[i] = get_zmbuf(vq);

	vhost_for_each_try_unroll(i, 0, PACKED_BATCH_SIZE) {
		if (!zmbufs[i])
			goto free_pkt;
	}

	vhost_for_each_try_unroll(i, 0, PACKED_BATCH_SIZE) {
		zmbufs[i]->mbuf = pkts[i];
		zmbufs[i]->desc_idx = ids[i];
		zmbufs[i]->desc_count = 1;
	}

	vhost_for_each_try_unroll(i, 0, PACKED_BATCH_SIZE)
		rte_mbuf_refcnt_update(pkts[i], 1);

	vhost_for_each_try_unroll(i, 0, PACKED_BATCH_SIZE)
		TAILQ_INSERT_TAIL(&vq->zmbuf_list, zmbufs[i], next);

	vq->nr_zmbuf += PACKED_BATCH_SIZE;
	vq_inc_last_avail_packed(vq, PACKED_BATCH_SIZE);

	return 0;

free_pkt:
	vhost_for_each_try_unroll(i, 0, PACKED_BATCH_SIZE)
		rte_pktmbuf_free(pkts[i]);

	return -1;
}