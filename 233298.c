unsigned int xdr_page_pos(const struct xdr_stream *xdr)
{
	unsigned int pos = xdr_stream_pos(xdr);

	WARN_ON(pos < xdr->buf->head[0].iov_len);
	return pos - xdr->buf->head[0].iov_len;
}