virtio_dev_rx_packed(struct virtio_net *dev,
		     struct vhost_virtqueue *vq,
		     struct rte_mbuf **pkts,
		     uint32_t count)
{
	uint32_t pkt_idx = 0;
	uint32_t remained = count;

	do {
		rte_prefetch0(&vq->desc_packed[vq->last_avail_idx]);

		if (remained >= PACKED_BATCH_SIZE) {
			if (!virtio_dev_rx_batch_packed(dev, vq,
							&pkts[pkt_idx])) {
				pkt_idx += PACKED_BATCH_SIZE;
				remained -= PACKED_BATCH_SIZE;
				continue;
			}
		}

		if (virtio_dev_rx_single_packed(dev, vq, pkts[pkt_idx]))
			break;
		pkt_idx++;
		remained--;

	} while (pkt_idx < count);

	if (vq->shadow_used_idx) {
		do_data_copy_enqueue(dev, vq);
		vhost_flush_enqueue_shadow_packed(dev, vq);
	}

	if (pkt_idx)
		vhost_vring_call_packed(dev, vq);

	return pkt_idx;
}