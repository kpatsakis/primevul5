virtio_dev_tx_single_packed_zmbuf(struct virtio_net *dev,
				  struct vhost_virtqueue *vq,
				  struct rte_mempool *mbuf_pool,
				  struct rte_mbuf **pkts)
{
	uint16_t buf_id, desc_count;
	struct zcopy_mbuf *zmbuf;

	if (vhost_dequeue_single_packed(dev, vq, mbuf_pool, pkts, &buf_id,
					&desc_count))
		return -1;

	zmbuf = get_zmbuf(vq);
	if (!zmbuf) {
		rte_pktmbuf_free(*pkts);
		return -1;
	}
	zmbuf->mbuf = *pkts;
	zmbuf->desc_idx = buf_id;
	zmbuf->desc_count = desc_count;

	rte_mbuf_refcnt_update(*pkts, 1);

	vq->nr_zmbuf += 1;
	TAILQ_INSERT_TAIL(&vq->zmbuf_list, zmbuf, next);

	vq_inc_last_avail_packed(vq, desc_count);
	return 0;
}