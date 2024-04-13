copy_vnet_hdr_from_desc(struct virtio_net_hdr *hdr,
		struct buf_vector *buf_vec)
{
	uint64_t len;
	uint64_t remain = sizeof(struct virtio_net_hdr);
	uint64_t src;
	uint64_t dst = (uint64_t)(uintptr_t)hdr;

	while (remain) {
		len = RTE_MIN(remain, buf_vec->buf_len);
		src = buf_vec->buf_addr;
		rte_memcpy((void *)(uintptr_t)dst,
				(void *)(uintptr_t)src, len);

		remain -= len;
		dst += len;
		buf_vec++;
	}
}