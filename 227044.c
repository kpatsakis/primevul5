copy_desc_to_mbuf(struct virtio_net *dev, struct vhost_virtqueue *vq,
		  struct buf_vector *buf_vec, uint16_t nr_vec,
		  struct rte_mbuf *m, struct rte_mempool *mbuf_pool)
{
	uint32_t buf_avail, buf_offset;
	uint64_t buf_addr, buf_iova, buf_len;
	uint32_t mbuf_avail, mbuf_offset;
	uint32_t cpy_len;
	struct rte_mbuf *cur = m, *prev = m;
	struct virtio_net_hdr tmp_hdr;
	struct virtio_net_hdr *hdr = NULL;
	/* A counter to avoid desc dead loop chain */
	uint16_t vec_idx = 0;
	struct batch_copy_elem *batch_copy = vq->batch_copy_elems;
	int error = 0;

	buf_addr = buf_vec[vec_idx].buf_addr;
	buf_iova = buf_vec[vec_idx].buf_iova;
	buf_len = buf_vec[vec_idx].buf_len;

	if (unlikely(buf_len < dev->vhost_hlen && nr_vec <= 1)) {
		error = -1;
		goto out;
	}

	if (virtio_net_with_host_offload(dev)) {
		if (unlikely(buf_len < sizeof(struct virtio_net_hdr))) {
			/*
			 * No luck, the virtio-net header doesn't fit
			 * in a contiguous virtual area.
			 */
			copy_vnet_hdr_from_desc(&tmp_hdr, buf_vec);
			hdr = &tmp_hdr;
		} else {
			hdr = (struct virtio_net_hdr *)((uintptr_t)buf_addr);
		}
	}

	/*
	 * A virtio driver normally uses at least 2 desc buffers
	 * for Tx: the first for storing the header, and others
	 * for storing the data.
	 */
	if (unlikely(buf_len < dev->vhost_hlen)) {
		buf_offset = dev->vhost_hlen - buf_len;
		vec_idx++;
		buf_addr = buf_vec[vec_idx].buf_addr;
		buf_iova = buf_vec[vec_idx].buf_iova;
		buf_len = buf_vec[vec_idx].buf_len;
		buf_avail  = buf_len - buf_offset;
	} else if (buf_len == dev->vhost_hlen) {
		if (unlikely(++vec_idx >= nr_vec))
			goto out;
		buf_addr = buf_vec[vec_idx].buf_addr;
		buf_iova = buf_vec[vec_idx].buf_iova;
		buf_len = buf_vec[vec_idx].buf_len;

		buf_offset = 0;
		buf_avail = buf_len;
	} else {
		buf_offset = dev->vhost_hlen;
		buf_avail = buf_vec[vec_idx].buf_len - dev->vhost_hlen;
	}

	PRINT_PACKET(dev,
			(uintptr_t)(buf_addr + buf_offset),
			(uint32_t)buf_avail, 0);

	mbuf_offset = 0;
	mbuf_avail  = m->buf_len - RTE_PKTMBUF_HEADROOM;
	while (1) {
		uint64_t hpa;

		cpy_len = RTE_MIN(buf_avail, mbuf_avail);

		/*
		 * A desc buf might across two host physical pages that are
		 * not continuous. In such case (gpa_to_hpa returns 0), data
		 * will be copied even though zero copy is enabled.
		 */
		if (unlikely(dev->dequeue_zero_copy && (hpa = gpa_to_hpa(dev,
					buf_iova + buf_offset, cpy_len)))) {
			cur->data_len = cpy_len;
			cur->data_off = 0;
			cur->buf_addr =
				(void *)(uintptr_t)(buf_addr + buf_offset);
			cur->buf_iova = hpa;

			/*
			 * In zero copy mode, one mbuf can only reference data
			 * for one or partial of one desc buff.
			 */
			mbuf_avail = cpy_len;
		} else {
			if (likely(cpy_len > MAX_BATCH_LEN ||
				   vq->batch_copy_nb_elems >= vq->size ||
				   (hdr && cur == m))) {
				rte_memcpy(rte_pktmbuf_mtod_offset(cur, void *,
								   mbuf_offset),
					   (void *)((uintptr_t)(buf_addr +
							   buf_offset)),
					   cpy_len);
			} else {
				batch_copy[vq->batch_copy_nb_elems].dst =
					rte_pktmbuf_mtod_offset(cur, void *,
								mbuf_offset);
				batch_copy[vq->batch_copy_nb_elems].src =
					(void *)((uintptr_t)(buf_addr +
								buf_offset));
				batch_copy[vq->batch_copy_nb_elems].len =
					cpy_len;
				vq->batch_copy_nb_elems++;
			}
		}

		mbuf_avail  -= cpy_len;
		mbuf_offset += cpy_len;
		buf_avail -= cpy_len;
		buf_offset += cpy_len;

		/* This buf reaches to its end, get the next one */
		if (buf_avail == 0) {
			if (++vec_idx >= nr_vec)
				break;

			buf_addr = buf_vec[vec_idx].buf_addr;
			buf_iova = buf_vec[vec_idx].buf_iova;
			buf_len = buf_vec[vec_idx].buf_len;

			buf_offset = 0;
			buf_avail  = buf_len;

			PRINT_PACKET(dev, (uintptr_t)buf_addr,
					(uint32_t)buf_avail, 0);
		}

		/*
		 * This mbuf reaches to its end, get a new one
		 * to hold more data.
		 */
		if (mbuf_avail == 0) {
			cur = rte_pktmbuf_alloc(mbuf_pool);
			if (unlikely(cur == NULL)) {
				VHOST_LOG_DATA(ERR, "Failed to "
					"allocate memory for mbuf.\n");
				error = -1;
				goto out;
			}
			if (unlikely(dev->dequeue_zero_copy))
				rte_mbuf_refcnt_update(cur, 1);

			prev->next = cur;
			prev->data_len = mbuf_offset;
			m->nb_segs += 1;
			m->pkt_len += mbuf_offset;
			prev = cur;

			mbuf_offset = 0;
			mbuf_avail  = cur->buf_len - RTE_PKTMBUF_HEADROOM;
		}
	}

	prev->data_len = mbuf_offset;
	m->pkt_len    += mbuf_offset;

	if (hdr)
		vhost_dequeue_offload(hdr, m);

out:

	return error;
}