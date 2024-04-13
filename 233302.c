static __be32 * __xdr_inline_decode(struct xdr_stream *xdr, size_t nbytes)
{
	unsigned int nwords = XDR_QUADLEN(nbytes);
	__be32 *p = xdr->p;
	__be32 *q = p + nwords;

	if (unlikely(nwords > xdr->nwords || q > xdr->end || q < p))
		return NULL;
	xdr->p = q;
	xdr->nwords -= nwords;
	return p;
}