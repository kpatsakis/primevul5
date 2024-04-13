fill_vec_buf_split(struct virtio_net *dev, struct vhost_virtqueue *vq,
			 uint32_t avail_idx, uint16_t *vec_idx,
			 struct buf_vector *buf_vec, uint16_t *desc_chain_head,
			 uint32_t *desc_chain_len, uint8_t perm)
{
	uint16_t idx = vq->avail->ring[avail_idx & (vq->size - 1)];
	uint16_t vec_id = *vec_idx;
	uint32_t len    = 0;
	uint64_t dlen;
	uint32_t nr_descs = vq->size;
	uint32_t cnt    = 0;
	struct vring_desc *descs = vq->desc;
	struct vring_desc *idesc = NULL;

	if (unlikely(idx >= vq->size))
		return -1;

	*desc_chain_head = idx;

	if (vq->desc[idx].flags & VRING_DESC_F_INDIRECT) {
		dlen = vq->desc[idx].len;
		nr_descs = dlen / sizeof(struct vring_desc);
		if (unlikely(nr_descs > vq->size))
			return -1;

		descs = (struct vring_desc *)(uintptr_t)
			vhost_iova_to_vva(dev, vq, vq->desc[idx].addr,
						&dlen,
						VHOST_ACCESS_RO);
		if (unlikely(!descs))
			return -1;

		if (unlikely(dlen < vq->desc[idx].len)) {
			/*
			 * The indirect desc table is not contiguous
			 * in process VA space, we have to copy it.
			 */
			idesc = vhost_alloc_copy_ind_table(dev, vq,
					vq->desc[idx].addr, vq->desc[idx].len);
			if (unlikely(!idesc))
				return -1;

			descs = idesc;
		}

		idx = 0;
	}

	while (1) {
		if (unlikely(idx >= nr_descs || cnt++ >= nr_descs)) {
			free_ind_table(idesc);
			return -1;
		}

		len += descs[idx].len;

		if (unlikely(map_one_desc(dev, vq, buf_vec, &vec_id,
						descs[idx].addr, descs[idx].len,
						perm))) {
			free_ind_table(idesc);
			return -1;
		}

		if ((descs[idx].flags & VRING_DESC_F_NEXT) == 0)
			break;

		idx = descs[idx].next;
	}

	*desc_chain_len = len;
	*vec_idx = vec_id;

	if (unlikely(!!idesc))
		free_ind_table(idesc);

	return 0;
}