virtio_dev_tx_packed_zmbuf(struct virtio_net *dev,
			   struct vhost_virtqueue *vq,
			   struct rte_mempool *mbuf_pool,
			   struct rte_mbuf **pkts,
			   uint32_t count)
{
	uint32_t pkt_idx = 0;
	uint32_t remained = count;

	free_zmbuf(vq);

	do {
		if (remained >= PACKED_BATCH_SIZE) {
			if (!virtio_dev_tx_batch_packed_zmbuf(dev, vq,
				mbuf_pool, &pkts[pkt_idx])) {
				pkt_idx += PACKED_BATCH_SIZE;
				remained -= PACKED_BATCH_SIZE;
				continue;
			}
		}

		if (virtio_dev_tx_single_packed_zmbuf(dev, vq, mbuf_pool,
						      &pkts[pkt_idx]))
			break;
		pkt_idx++;
		remained--;

	} while (remained);

	if (pkt_idx)
		vhost_vring_call_packed(dev, vq);

	return pkt_idx;
}