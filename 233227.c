static __be32 *xdr_get_next_encode_buffer(struct xdr_stream *xdr,
		size_t nbytes)
{
	__be32 *p;
	int space_left;
	int frag1bytes, frag2bytes;

	if (nbytes > PAGE_SIZE)
		goto out_overflow; /* Bigger buffers require special handling */
	if (xdr->buf->len + nbytes > xdr->buf->buflen)
		goto out_overflow; /* Sorry, we're totally out of space */
	frag1bytes = (xdr->end - xdr->p) << 2;
	frag2bytes = nbytes - frag1bytes;
	if (xdr->iov)
		xdr->iov->iov_len += frag1bytes;
	else
		xdr->buf->page_len += frag1bytes;
	xdr->page_ptr++;
	xdr->iov = NULL;
	/*
	 * If the last encode didn't end exactly on a page boundary, the
	 * next one will straddle boundaries.  Encode into the next
	 * page, then copy it back later in xdr_commit_encode.  We use
	 * the "scratch" iov to track any temporarily unused fragment of
	 * space at the end of the previous buffer:
	 */
	xdr_set_scratch_buffer(xdr, xdr->p, frag1bytes);
	p = page_address(*xdr->page_ptr);
	/*
	 * Note this is where the next encode will start after we've
	 * shifted this one back:
	 */
	xdr->p = (void *)p + frag2bytes;
	space_left = xdr->buf->buflen - xdr->buf->len;
	xdr->end = (void *)p + min_t(int, space_left, PAGE_SIZE);
	xdr->buf->page_len += frag2bytes;
	xdr->buf->len += nbytes;
	return p;
out_overflow:
	trace_rpc_xdr_overflow(xdr, nbytes);
	return NULL;
}