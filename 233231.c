static void xdr_buf_head_shift_right(const struct xdr_buf *buf,
				     unsigned int base, unsigned int len,
				     unsigned int shift)
{
	const struct kvec *head = buf->head;

	if (!shift)
		return;
	if (base >= head->iov_len) {
		xdr_buf_pages_shift_right(buf, head->iov_len - base, len,
					  shift);
		return;
	}
	if (base + len > head->iov_len)
		xdr_buf_pages_shift_right(buf, 0, base + len - head->iov_len,
					  shift);
	xdr_buf_head_copy_right(buf, base, len, shift);
}