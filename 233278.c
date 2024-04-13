int xdr_reserve_space_vec(struct xdr_stream *xdr, struct kvec *vec, size_t nbytes)
{
	int thislen;
	int v = 0;
	__be32 *p;

	/*
	 * svcrdma requires every READ payload to start somewhere
	 * in xdr->pages.
	 */
	if (xdr->iov == xdr->buf->head) {
		xdr->iov = NULL;
		xdr->end = xdr->p;
	}

	while (nbytes) {
		thislen = xdr->buf->page_len % PAGE_SIZE;
		thislen = min_t(size_t, nbytes, PAGE_SIZE - thislen);

		p = xdr_reserve_space(xdr, thislen);
		if (!p)
			return -EIO;

		vec[v].iov_base = p;
		vec[v].iov_len = thislen;
		v++;
		nbytes -= thislen;
	}

	return v;
}