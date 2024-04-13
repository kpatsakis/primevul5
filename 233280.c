__be32 * xdr_inline_decode(struct xdr_stream *xdr, size_t nbytes)
{
	__be32 *p;

	if (unlikely(nbytes == 0))
		return xdr->p;
	if (xdr->p == xdr->end && !xdr_set_next_buffer(xdr))
		goto out_overflow;
	p = __xdr_inline_decode(xdr, nbytes);
	if (p != NULL)
		return p;
	return xdr_copy_to_scratch(xdr, nbytes);
out_overflow:
	trace_rpc_xdr_overflow(xdr, nbytes);
	return NULL;
}