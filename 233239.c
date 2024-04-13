inline void xdr_commit_encode(struct xdr_stream *xdr)
{
	int shift = xdr->scratch.iov_len;
	void *page;

	if (shift == 0)
		return;
	page = page_address(*xdr->page_ptr);
	memcpy(xdr->scratch.iov_base, page, shift);
	memmove(page, page + shift, (void *)xdr->p - page);
	xdr_reset_scratch_buffer(xdr);
}