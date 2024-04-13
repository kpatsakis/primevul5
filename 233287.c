unsigned int xdr_align_data(struct xdr_stream *xdr, unsigned int offset,
			    unsigned int length)
{
	struct xdr_buf *buf = xdr->buf;
	unsigned int from, bytes, len;
	unsigned int shift;

	xdr_realign_pages(xdr);
	from = xdr_page_pos(xdr);

	if (from >= buf->page_len + buf->tail->iov_len)
		return 0;
	if (from + buf->head->iov_len >= buf->len)
		return 0;

	len = buf->len - buf->head->iov_len;

	/* We only shift data left! */
	if (WARN_ONCE(from < offset, "SUNRPC: misaligned data src=%u dst=%u\n",
		      from, offset))
		return 0;
	if (WARN_ONCE(offset > buf->page_len,
		      "SUNRPC: buffer overflow. offset=%u, page_len=%u\n",
		      offset, buf->page_len))
		return 0;

	/* Move page data to the left */
	shift = from - offset;
	xdr_buf_pages_shift_left(buf, from, len, shift);

	bytes = xdr_stream_remaining(xdr);
	if (length > bytes)
		length = bytes;
	bytes -= length;

	xdr->buf->len -= shift;
	xdr_set_page(xdr, offset + length, bytes);
	return length;
}