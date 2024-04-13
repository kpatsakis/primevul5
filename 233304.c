static unsigned int xdr_buf_pages_fill_sparse(const struct xdr_buf *buf,
					      unsigned int buflen, gfp_t gfp)
{
	unsigned int i, npages, pagelen;

	if (!(buf->flags & XDRBUF_SPARSE_PAGES))
		return buflen;
	if (buflen <= buf->head->iov_len)
		return buflen;
	pagelen = buflen - buf->head->iov_len;
	if (pagelen > buf->page_len)
		pagelen = buf->page_len;
	npages = (pagelen + buf->page_base + PAGE_SIZE - 1) >> PAGE_SHIFT;
	for (i = 0; i < npages; i++) {
		if (!buf->pages[i])
			continue;
		buf->pages[i] = alloc_page(gfp);
		if (likely(buf->pages[i]))
			continue;
		buflen -= pagelen;
		pagelen = i << PAGE_SHIFT;
		if (pagelen > buf->page_base)
			buflen += pagelen - buf->page_base;
		break;
	}
	return buflen;
}