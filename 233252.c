static unsigned int xdr_shrink_pagelen(struct xdr_buf *buf, unsigned int len)
{
	unsigned int shift, buflen = buf->len - buf->head->iov_len;

	WARN_ON_ONCE(len > buf->page_len);
	if (buf->head->iov_len >= buf->len || len > buflen)
		buflen = len;
	if (buf->page_len > buflen) {
		buf->buflen -= buf->page_len - buflen;
		buf->page_len = buflen;
	}
	if (len >= buf->page_len)
		return 0;
	shift = buf->page_len - len;
	xdr_buf_try_expand(buf, shift);
	xdr_buf_pages_shift_right(buf, len, buflen - len, shift);
	buf->page_len = len;
	buf->len -= shift;
	buf->buflen -= shift;
	return shift;
}