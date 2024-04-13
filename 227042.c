copy_vnet_hdr_to_desc(struct virtio_net *dev, struct vhost_virtqueue *vq,
		struct buf_vector *buf_vec,
		struct virtio_net_hdr_mrg_rxbuf *hdr)
{
	uint64_t len;
	uint64_t remain = dev->vhost_hlen;
	uint64_t src = (uint64_t)(uintptr_t)hdr, dst;
	uint64_t iova = buf_vec->buf_iova;

	while (remain) {
		len = RTE_MIN(remain,
				buf_vec->buf_len);
		dst = buf_vec->buf_addr;
		rte_memcpy((void *)(uintptr_t)dst,
				(void *)(uintptr_t)src,
				len);

		PRINT_PACKET(dev, (uintptr_t)dst,
				(uint32_t)len, 0);
		vhost_log_cache_write_iova(dev, vq,
				iova, len);

		remain -= len;
		iova += len;
		src += len;
		buf_vec++;
	}
}