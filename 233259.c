static void xdr_stream_set_pos(struct xdr_stream *xdr, unsigned int pos)
{
	unsigned int blen = xdr->buf->len;

	xdr->nwords = blen > pos ? XDR_QUADLEN(blen) - XDR_QUADLEN(pos) : 0;
}