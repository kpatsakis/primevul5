fill_vec_buf_packed_indirect(struct virtio_net *dev,
			struct vhost_virtqueue *vq,
			struct vring_packed_desc *desc, uint16_t *vec_idx,
			struct buf_vector *buf_vec, uint32_t *len, uint8_t perm)
{
	uint16_t i;
	uint32_t nr_descs;
	uint16_t vec_id = *vec_idx;
	uint64_t dlen;
	struct vring_packed_desc *descs, *idescs = NULL;

	dlen = desc->len;
	descs = (struct vring_packed_desc *)(uintptr_t)
		vhost_iova_to_vva(dev, vq, desc->addr, &dlen, VHOST_ACCESS_RO);
	if (unlikely(!descs))
		return -1;

	if (unlikely(dlen < desc->len)) {
		/*
		 * The indirect desc table is not contiguous
		 * in process VA space, we have to copy it.
		 */
		idescs = vhost_alloc_copy_ind_table(dev,
				vq, desc->addr, desc->len);
		if (unlikely(!idescs))
			return -1;

		descs = idescs;
	}

	nr_descs =  desc->len / sizeof(struct vring_packed_desc);
	if (unlikely(nr_descs >= vq->size)) {
		free_ind_table(idescs);
		return -1;
	}

	for (i = 0; i < nr_descs; i++) {
		if (unlikely(vec_id >= BUF_VECTOR_MAX)) {
			free_ind_table(idescs);
			return -1;
		}

		*len += descs[i].len;
		if (unlikely(map_one_desc(dev, vq, buf_vec, &vec_id,
						descs[i].addr, descs[i].len,
						perm)))
			return -1;
	}
	*vec_idx = vec_id;

	if (unlikely(!!idescs))
		free_ind_table(idescs);

	return 0;
}