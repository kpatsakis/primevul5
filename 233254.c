static unsigned int xdr_shrink_bufhead(struct xdr_buf *buf, unsigned int len)
{
	struct kvec *head = buf->head;
	unsigned int shift, buflen = max(buf->len, len);

	WARN_ON_ONCE(len > head->iov_len);
	if (head->iov_len > buflen) {
		buf->buflen -= head->iov_len - buflen;
		head->iov_len = buflen;
	}
	if (len >= head->iov_len)
		return 0;
	shift = head->iov_len - len;
	xdr_buf_try_expand(buf, shift);
	xdr_buf_head_shift_right(buf, len, buflen - len, shift);
	head->iov_len = len;
	buf->buflen -= shift;
	buf->len -= shift;
	return shift;
}