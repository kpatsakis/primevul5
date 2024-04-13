static __be32 *xdr_copy_to_scratch(struct xdr_stream *xdr, size_t nbytes)
{
	__be32 *p;
	char *cpdest = xdr->scratch.iov_base;
	size_t cplen = (char *)xdr->end - (char *)xdr->p;

	if (nbytes > xdr->scratch.iov_len)
		goto out_overflow;
	p = __xdr_inline_decode(xdr, cplen);
	if (p == NULL)
		return NULL;
	memcpy(cpdest, p, cplen);
	if (!xdr_set_next_buffer(xdr))
		goto out_overflow;
	cpdest += cplen;
	nbytes -= cplen;
	p = __xdr_inline_decode(xdr, nbytes);
	if (p == NULL)
		return NULL;
	memcpy(cpdest, p, nbytes);
	return xdr->scratch.iov_base;
out_overflow:
	trace_rpc_xdr_overflow(xdr, nbytes);
	return NULL;
}