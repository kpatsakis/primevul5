rte_vhost_dequeue_burst(int vid, uint16_t queue_id,
	struct rte_mempool *mbuf_pool, struct rte_mbuf **pkts, uint16_t count)
{
	struct virtio_net *dev;
	struct rte_mbuf *rarp_mbuf = NULL;
	struct vhost_virtqueue *vq;
	int16_t success = 1;

	dev = get_device(vid);
	if (!dev)
		return 0;

	if (unlikely(!(dev->flags & VIRTIO_DEV_BUILTIN_VIRTIO_NET))) {
		VHOST_LOG_DATA(ERR,
			"(%d) %s: built-in vhost net backend is disabled.\n",
			dev->vid, __func__);
		return 0;
	}

	if (unlikely(!is_valid_virt_queue_idx(queue_id, 1, dev->nr_vring))) {
		VHOST_LOG_DATA(ERR,
			"(%d) %s: invalid virtqueue idx %d.\n",
			dev->vid, __func__, queue_id);
		return 0;
	}

	vq = dev->virtqueue[queue_id];

	if (unlikely(rte_spinlock_trylock(&vq->access_lock) == 0))
		return 0;

	if (unlikely(vq->enabled == 0)) {
		count = 0;
		goto out_access_unlock;
	}

	if (dev->features & (1ULL << VIRTIO_F_IOMMU_PLATFORM))
		vhost_user_iotlb_rd_lock(vq);

	if (unlikely(vq->access_ok == 0))
		if (unlikely(vring_translate(dev, vq) < 0)) {
			count = 0;
			goto out;
		}

	/*
	 * Construct a RARP broadcast packet, and inject it to the "pkts"
	 * array, to looks like that guest actually send such packet.
	 *
	 * Check user_send_rarp() for more information.
	 *
	 * broadcast_rarp shares a cacheline in the virtio_net structure
	 * with some fields that are accessed during enqueue and
	 * __atomic_compare_exchange_n causes a write if performed compare
	 * and exchange. This could result in false sharing between enqueue
	 * and dequeue.
	 *
	 * Prevent unnecessary false sharing by reading broadcast_rarp first
	 * and only performing compare and exchange if the read indicates it
	 * is likely to be set.
	 */
	if (unlikely(__atomic_load_n(&dev->broadcast_rarp, __ATOMIC_ACQUIRE) &&
			__atomic_compare_exchange_n(&dev->broadcast_rarp,
			&success, 0, 0, __ATOMIC_RELEASE, __ATOMIC_RELAXED))) {

		rarp_mbuf = rte_net_make_rarp_packet(mbuf_pool, &dev->mac);
		if (rarp_mbuf == NULL) {
			VHOST_LOG_DATA(ERR, "Failed to make RARP packet.\n");
			count = 0;
			goto out;
		}
		count -= 1;
	}

	if (vq_is_packed(dev)) {
		if (unlikely(dev->dequeue_zero_copy))
			count = virtio_dev_tx_packed_zmbuf(dev, vq, mbuf_pool,
							   pkts, count);
		else
			count = virtio_dev_tx_packed(dev, vq, mbuf_pool, pkts,
						     count);
	} else
		count = virtio_dev_tx_split(dev, vq, mbuf_pool, pkts, count);

out:
	if (dev->features & (1ULL << VIRTIO_F_IOMMU_PLATFORM))
		vhost_user_iotlb_rd_unlock(vq);

out_access_unlock:
	rte_spinlock_unlock(&vq->access_lock);

	if (unlikely(rarp_mbuf != NULL)) {
		/*
		 * Inject it to the head of "pkts" array, so that switch's mac
		 * learning table will get updated first.
		 */
		memmove(&pkts[1], pkts, count * sizeof(struct rte_mbuf *));
		pkts[0] = rarp_mbuf;
		count += 1;
	}

	return count;
}