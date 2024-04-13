virtio_dev_rx_single_packed(struct virtio_net *dev,
			    struct vhost_virtqueue *vq,
			    struct rte_mbuf *pkt)
{
	struct buf_vector buf_vec[BUF_VECTOR_MAX];
	uint16_t nr_descs = 0;

	rte_smp_rmb();
	if (unlikely(vhost_enqueue_single_packed(dev, vq, pkt, buf_vec,
						 &nr_descs) < 0)) {
		VHOST_LOG_DATA(DEBUG,
				"(%d) failed to get enough desc from vring\n",
				dev->vid);
		return -1;
	}

	VHOST_LOG_DATA(DEBUG, "(%d) current index %d | end index %d\n",
			dev->vid, vq->last_avail_idx,
			vq->last_avail_idx + nr_descs);

	vq_inc_last_avail_packed(vq, nr_descs);

	return 0;
}