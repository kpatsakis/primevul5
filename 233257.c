static void xdr_buf_pages_shift_right(const struct xdr_buf *buf,
				      unsigned int base, unsigned int len,
				      unsigned int shift)
{
	if (!shift || !len)
		return;
	if (base >= buf->page_len) {
		xdr_buf_tail_shift_right(buf, base - buf->page_len, len, shift);
		return;
	}
	if (base + len > buf->page_len)
		xdr_buf_tail_shift_right(buf, 0, base + len - buf->page_len,
					 shift);
	xdr_buf_pages_copy_right(buf, base, len, shift);
}