__be32 * xdr_reserve_space(struct xdr_stream *xdr, size_t nbytes)
{
	__be32 *p = xdr->p;
	__be32 *q;

	xdr_commit_encode(xdr);
	/* align nbytes on the next 32-bit boundary */
	nbytes += 3;
	nbytes &= ~3;
	q = p + (nbytes >> 2);
	if (unlikely(q > xdr->end || q < p))
		return xdr_get_next_encode_buffer(xdr, nbytes);
	xdr->p = q;
	if (xdr->iov)
		xdr->iov->iov_len += nbytes;
	else
		xdr->buf->page_len += nbytes;
	xdr->buf->len += nbytes;
	return p;
}