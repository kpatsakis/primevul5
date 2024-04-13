int xdr_buf_subsegment(const struct xdr_buf *buf, struct xdr_buf *subbuf,
		       unsigned int base, unsigned int len)
{
	subbuf->buflen = subbuf->len = len;
	if (base < buf->head[0].iov_len) {
		subbuf->head[0].iov_base = buf->head[0].iov_base + base;
		subbuf->head[0].iov_len = min_t(unsigned int, len,
						buf->head[0].iov_len - base);
		len -= subbuf->head[0].iov_len;
		base = 0;
	} else {
		base -= buf->head[0].iov_len;
		subbuf->head[0].iov_base = buf->head[0].iov_base;
		subbuf->head[0].iov_len = 0;
	}

	if (base < buf->page_len) {
		subbuf->page_len = min(buf->page_len - base, len);
		base += buf->page_base;
		subbuf->page_base = base & ~PAGE_MASK;
		subbuf->pages = &buf->pages[base >> PAGE_SHIFT];
		len -= subbuf->page_len;
		base = 0;
	} else {
		base -= buf->page_len;
		subbuf->pages = buf->pages;
		subbuf->page_base = 0;
		subbuf->page_len = 0;
	}

	if (base < buf->tail[0].iov_len) {
		subbuf->tail[0].iov_base = buf->tail[0].iov_base + base;
		subbuf->tail[0].iov_len = min_t(unsigned int, len,
						buf->tail[0].iov_len - base);
		len -= subbuf->tail[0].iov_len;
		base = 0;
	} else {
		base -= buf->tail[0].iov_len;
		subbuf->tail[0].iov_base = buf->tail[0].iov_base;
		subbuf->tail[0].iov_len = 0;
	}

	if (base || len)
		return -1;
	return 0;
}