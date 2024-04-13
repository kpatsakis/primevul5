unsigned int xdr_expand_hole(struct xdr_stream *xdr, unsigned int offset,
			     unsigned int length)
{
	struct xdr_buf *buf = xdr->buf;
	unsigned int from, to, shift;

	xdr_realign_pages(xdr);
	from = xdr_page_pos(xdr);
	to = xdr_align_size(offset + length);

	/* Could the hole be behind us? */
	if (to > from) {
		unsigned int buflen = buf->len - buf->head->iov_len;
		shift = to - from;
		xdr_buf_try_expand(buf, shift);
		xdr_buf_pages_shift_right(buf, from, buflen, shift);
		xdr_set_page(xdr, to, xdr_stream_remaining(xdr));
	} else if (to != from)
		xdr_align_data(xdr, to, 0);
	xdr_buf_pages_zero(buf, offset, length);

	return length;
}