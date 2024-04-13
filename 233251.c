static void xdr_buf_tail_shift_left(const struct xdr_buf *buf,
				    unsigned int base, unsigned int len,
				    unsigned int shift)
{
	if (!shift || !len)
		return;
	xdr_buf_tail_copy_left(buf, base, len, shift);
}