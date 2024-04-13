static void xdr_stream_page_set_pos(struct xdr_stream *xdr, unsigned int pos)
{
	xdr_stream_set_pos(xdr, pos + xdr->buf->head[0].iov_len);
}