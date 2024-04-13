void xdr_init_decode(struct xdr_stream *xdr, struct xdr_buf *buf, __be32 *p,
		     struct rpc_rqst *rqst)
{
	xdr->buf = buf;
	xdr_reset_scratch_buffer(xdr);
	xdr->nwords = XDR_QUADLEN(buf->len);
	if (xdr_set_iov(xdr, buf->head, 0, buf->len) == 0 &&
	    xdr_set_page_base(xdr, 0, buf->len) == 0)
		xdr_set_iov(xdr, buf->tail, 0, buf->len);
	if (p != NULL && p > xdr->p && xdr->end >= p) {
		xdr->nwords -= p - xdr->p;
		xdr->p = p;
	}
	xdr->rqst = rqst;
}