unsigned int xdr_stream_pos(const struct xdr_stream *xdr)
{
	return (unsigned int)(XDR_QUADLEN(xdr->buf->len) - xdr->nwords) << 2;
}