virtio_dev_tx_batch_packed(struct virtio_net *dev,
			   struct vhost_virtqueue *vq,
			   struct rte_mempool *mbuf_pool,
			   struct rte_mbuf **pkts)
{
	uint16_t avail_idx = vq->last_avail_idx;
	uint32_t buf_offset = dev->vhost_hlen;
	uintptr_t desc_addrs[PACKED_BATCH_SIZE];
	uint16_t ids[PACKED_BATCH_SIZE];
	uint16_t i;

	if (vhost_reserve_avail_batch_packed(dev, vq, mbuf_pool, pkts,
					     avail_idx, desc_addrs, ids))
		return -1;

	vhost_for_each_try_unroll(i, 0, PACKED_BATCH_SIZE)
		rte_prefetch0((void *)(uintptr_t)desc_addrs[i]);

	vhost_for_each_try_unroll(i, 0, PACKED_BATCH_SIZE)
		rte_memcpy(rte_pktmbuf_mtod_offset(pkts[i], void *, 0),
			   (void *)(uintptr_t)(desc_addrs[i] + buf_offset),
			   pkts[i]->pkt_len);

	if (virtio_net_is_inorder(dev))
		vhost_shadow_dequeue_batch_packed_inorder(vq,
			ids[PACKED_BATCH_SIZE - 1]);
	else
		vhost_shadow_dequeue_batch_packed(dev, vq, ids);

	vq_inc_last_avail_packed(vq, PACKED_BATCH_SIZE);

	return 0;
}