virtio_dev_extbuf_alloc(struct rte_mbuf *pkt, uint32_t size)
{
	struct rte_mbuf_ext_shared_info *shinfo = NULL;
	uint32_t total_len = RTE_PKTMBUF_HEADROOM + size;
	uint16_t buf_len;
	rte_iova_t iova;
	void *buf;

	/* Try to use pkt buffer to store shinfo to reduce the amount of memory
	 * required, otherwise store shinfo in the new buffer.
	 */
	if (rte_pktmbuf_tailroom(pkt) >= sizeof(*shinfo))
		shinfo = rte_pktmbuf_mtod(pkt,
					  struct rte_mbuf_ext_shared_info *);
	else {
		total_len += sizeof(*shinfo) + sizeof(uintptr_t);
		total_len = RTE_ALIGN_CEIL(total_len, sizeof(uintptr_t));
	}

	if (unlikely(total_len > UINT16_MAX))
		return -ENOSPC;

	buf_len = total_len;
	buf = rte_malloc(NULL, buf_len, RTE_CACHE_LINE_SIZE);
	if (unlikely(buf == NULL))
		return -ENOMEM;

	/* Initialize shinfo */
	if (shinfo) {
		shinfo->free_cb = virtio_dev_extbuf_free;
		shinfo->fcb_opaque = buf;
		rte_mbuf_ext_refcnt_set(shinfo, 1);
	} else {
		shinfo = rte_pktmbuf_ext_shinfo_init_helper(buf, &buf_len,
					      virtio_dev_extbuf_free, buf);
		if (unlikely(shinfo == NULL)) {
			rte_free(buf);
			VHOST_LOG_DATA(ERR, "Failed to init shinfo\n");
			return -1;
		}
	}

	iova = rte_malloc_virt2iova(buf);
	rte_pktmbuf_attach_extbuf(pkt, buf, iova, buf_len, shinfo);
	rte_pktmbuf_reset_headroom(pkt);

	return 0;
}