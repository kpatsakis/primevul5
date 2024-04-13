void xdr_buf_trim(struct xdr_buf *buf, unsigned int len)
{
	size_t cur;
	unsigned int trim = len;

	if (buf->tail[0].iov_len) {
		cur = min_t(size_t, buf->tail[0].iov_len, trim);
		buf->tail[0].iov_len -= cur;
		trim -= cur;
		if (!trim)
			goto fix_len;
	}

	if (buf->page_len) {
		cur = min_t(unsigned int, buf->page_len, trim);
		buf->page_len -= cur;
		trim -= cur;
		if (!trim)
			goto fix_len;
	}

	if (buf->head[0].iov_len) {
		cur = min_t(size_t, buf->head[0].iov_len, trim);
		buf->head[0].iov_len -= cur;
		trim -= cur;
	}
fix_len:
	buf->len -= (len - trim);
}