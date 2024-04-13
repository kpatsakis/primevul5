static __be32 *__xdr_inline_decode(struct xdr_stream *xdr, size_t nbytes)
{
	__be32 *p = xdr->p;
	__be32 *q = p + XDR_QUADLEN(nbytes);

        if (q > xdr->end || q < p)
		return NULL;
	xdr->p = q;
	return p;
}
