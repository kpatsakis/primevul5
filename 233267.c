size_t xdr_buf_pagecount(const struct xdr_buf *buf)
{
	if (!buf->page_len)
		return 0;
	return (buf->page_base + buf->page_len + PAGE_SIZE - 1) >> PAGE_SHIFT;
}