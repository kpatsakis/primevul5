static void xdr_set_next_page(struct xdr_stream *xdr)
{
	unsigned int newbase;

	newbase = (1 + xdr->page_ptr - xdr->buf->pages) << PAGE_SHIFT;
	newbase -= xdr->buf->page_base;
	if (newbase < xdr->buf->page_len)
		xdr_set_page_base(xdr, newbase, xdr_stream_remaining(xdr));
	else
		xdr_set_tail_base(xdr, 0, xdr_stream_remaining(xdr));
}