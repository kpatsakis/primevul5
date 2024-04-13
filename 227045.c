copy_mbuf_to_desc(struct virtio_net *dev, struct vhost_virtqueue *vq,
			    struct rte_mbuf *m, struct buf_vector *buf_vec,
			    uint16_t nr_vec, uint16_t num_buffers)
{
	uint32_t vec_idx = 0;
	uint32_t mbuf_offset, mbuf_avail;
	uint32_t buf_offset, buf_avail;
	uint64_t buf_addr, buf_iova, buf_len;
	uint32_t cpy_len;
	uint64_t hdr_addr;
	struct rte_mbuf *hdr_mbuf;
	struct batch_copy_elem *batch_copy = vq->batch_copy_elems;
	struct virtio_net_hdr_mrg_rxbuf tmp_hdr, *hdr = NULL;
	int error = 0;

	if (unlikely(m == NULL)) {
		error = -1;
		goto out;
	}

	buf_addr = buf_vec[vec_idx].buf_addr;
	buf_iova = buf_vec[vec_idx].buf_iova;
	buf_len = buf_vec[vec_idx].buf_len;

	if (unlikely(buf_len < dev->vhost_hlen && nr_vec <= 1)) {
		error = -1;
		goto out;
	}

	hdr_mbuf = m;
	hdr_addr = buf_addr;
	if (unlikely(buf_len < dev->vhost_hlen))
		hdr = &tmp_hdr;
	else
		hdr = (struct virtio_net_hdr_mrg_rxbuf *)(uintptr_t)hdr_addr;

	VHOST_LOG_DATA(DEBUG, "(%d) RX: num merge buffers %d\n",
		dev->vid, num_buffers);

	if (unlikely(buf_len < dev->vhost_hlen)) {
		buf_offset = dev->vhost_hlen - buf_len;
		vec_idx++;
		buf_addr = buf_vec[vec_idx].buf_addr;
		buf_iova = buf_vec[vec_idx].buf_iova;
		buf_len = buf_vec[vec_idx].buf_len;
		buf_avail = buf_len - buf_offset;
	} else {
		buf_offset = dev->vhost_hlen;
		buf_avail = buf_len - dev->vhost_hlen;
	}

	mbuf_avail  = rte_pktmbuf_data_len(m);
	mbuf_offset = 0;
	while (mbuf_avail != 0 || m->next != NULL) {
		/* done with current buf, get the next one */
		if (buf_avail == 0) {
			vec_idx++;
			if (unlikely(vec_idx >= nr_vec)) {
				error = -1;
				goto out;
			}

			buf_addr = buf_vec[vec_idx].buf_addr;
			buf_iova = buf_vec[vec_idx].buf_iova;
			buf_len = buf_vec[vec_idx].buf_len;

			buf_offset = 0;
			buf_avail  = buf_len;
		}

		/* done with current mbuf, get the next one */
		if (mbuf_avail == 0) {
			m = m->next;

			mbuf_offset = 0;
			mbuf_avail  = rte_pktmbuf_data_len(m);
		}

		if (hdr_addr) {
			virtio_enqueue_offload(hdr_mbuf, &hdr->hdr);
			if (rxvq_is_mergeable(dev))
				ASSIGN_UNLESS_EQUAL(hdr->num_buffers,
						num_buffers);

			if (unlikely(hdr == &tmp_hdr)) {
				copy_vnet_hdr_to_desc(dev, vq, buf_vec, hdr);
			} else {
				PRINT_PACKET(dev, (uintptr_t)hdr_addr,
						dev->vhost_hlen, 0);
				vhost_log_cache_write_iova(dev, vq,
						buf_vec[0].buf_iova,
						dev->vhost_hlen);
			}

			hdr_addr = 0;
		}

		cpy_len = RTE_MIN(buf_avail, mbuf_avail);

		if (likely(cpy_len > MAX_BATCH_LEN ||
					vq->batch_copy_nb_elems >= vq->size)) {
			rte_memcpy((void *)((uintptr_t)(buf_addr + buf_offset)),
				rte_pktmbuf_mtod_offset(m, void *, mbuf_offset),
				cpy_len);
			vhost_log_cache_write_iova(dev, vq,
						   buf_iova + buf_offset,
						   cpy_len);
			PRINT_PACKET(dev, (uintptr_t)(buf_addr + buf_offset),
				cpy_len, 0);
		} else {
			batch_copy[vq->batch_copy_nb_elems].dst =
				(void *)((uintptr_t)(buf_addr + buf_offset));
			batch_copy[vq->batch_copy_nb_elems].src =
				rte_pktmbuf_mtod_offset(m, void *, mbuf_offset);
			batch_copy[vq->batch_copy_nb_elems].log_addr =
				buf_iova + buf_offset;
			batch_copy[vq->batch_copy_nb_elems].len = cpy_len;
			vq->batch_copy_nb_elems++;
		}

		mbuf_avail  -= cpy_len;
		mbuf_offset += cpy_len;
		buf_avail  -= cpy_len;
		buf_offset += cpy_len;
	}

out:

	return error;
}