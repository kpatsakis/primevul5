bool xdr_stream_subsegment(struct xdr_stream *xdr, struct xdr_buf *subbuf,
			   unsigned int nbytes)
{
	unsigned int remaining, offset, len;

	if (xdr_buf_subsegment(xdr->buf, subbuf, xdr_stream_pos(xdr), nbytes))
		return false;

	if (subbuf->head[0].iov_len)
		if (!__xdr_inline_decode(xdr, subbuf->head[0].iov_len))
			return false;

	remaining = subbuf->page_len;
	offset = subbuf->page_base;
	while (remaining) {
		len = min_t(unsigned int, remaining, PAGE_SIZE) - offset;

		if (xdr->p == xdr->end && !xdr_set_next_buffer(xdr))
			return false;
		if (!__xdr_inline_decode(xdr, len))
			return false;

		remaining -= len;
		offset = 0;
	}

	return true;
}