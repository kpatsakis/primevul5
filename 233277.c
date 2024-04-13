static void xdr_buf_pages_shift_left(const struct xdr_buf *buf,
				     unsigned int base, unsigned int len,
				     unsigned int shift)
{
	if (!shift || !len)
		return;
	if (base >= buf->page_len) {
		xdr_buf_tail_shift_left(buf, base - buf->page_len, len, shift);
		return;
	}
	xdr_buf_pages_copy_left(buf, base, len, shift);
	len += base;
	if (len <= buf->page_len)
		return;
	xdr_buf_tail_copy_left(buf, 0, len - buf->page_len, shift);
}