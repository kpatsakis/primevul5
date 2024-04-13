static void xdr_buf_tail_shift_right(const struct xdr_buf *buf,
				     unsigned int base, unsigned int len,
				     unsigned int shift)
{
	const struct kvec *tail = buf->tail;

	if (base >= tail->iov_len || !shift || !len)
		return;
	xdr_buf_tail_copy_right(buf, base, len, shift);
}