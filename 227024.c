virtio_dev_tx_single_packed(struct virtio_net *dev,
			    struct vhost_virtqueue *vq,
			    struct rte_mempool *mbuf_pool,
			    struct rte_mbuf **pkts)
{

	uint16_t buf_id, desc_count;

	if (vhost_dequeue_single_packed(dev, vq, mbuf_pool, pkts, &buf_id,
					&desc_count))
		return -1;

	if (virtio_net_is_inorder(dev))
		vhost_shadow_dequeue_single_packed_inorder(vq, buf_id,
							   desc_count);
	else
		vhost_shadow_dequeue_single_packed(vq, buf_id, desc_count);

	vq_inc_last_avail_packed(vq, desc_count);

	return 0;
}