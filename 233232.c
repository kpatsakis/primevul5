static unsigned int xdr_set_tail_base(struct xdr_stream *xdr,
				      unsigned int base, unsigned int len)
{
	struct xdr_buf *buf = xdr->buf;

	xdr_stream_set_pos(xdr, base + buf->page_len + buf->head->iov_len);
	return xdr_set_iov(xdr, buf->tail, base, len);
}