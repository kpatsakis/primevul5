static unsigned int xdr_set_iov(struct xdr_stream *xdr, struct kvec *iov,
				unsigned int base, unsigned int len)
{
	if (len > iov->iov_len)
		len = iov->iov_len;
	if (unlikely(base > len))
		base = len;
	xdr->p = (__be32*)(iov->iov_base + base);
	xdr->end = (__be32*)(iov->iov_base + len);
	xdr->iov = iov;
	xdr->page_ptr = NULL;
	return len - base;
}