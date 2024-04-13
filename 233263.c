static void xdr_buf_tail_copy_right(const struct xdr_buf *buf,
				    unsigned int base, unsigned int len,
				    unsigned int shift)
{
	const struct kvec *tail = buf->tail;
	unsigned int to = base + shift;

	if (to >= tail->iov_len)
		return;
	if (len + to > tail->iov_len)
		len = tail->iov_len - to;
	memmove(tail->iov_base + to, tail->iov_base + base, len);
}