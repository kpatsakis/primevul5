static void xdr_set_page(struct xdr_stream *xdr, unsigned int base,
			 unsigned int len)
{
	if (xdr_set_page_base(xdr, base, len) == 0) {
		base -= xdr->buf->page_len;
		xdr_set_tail_base(xdr, base, len);
	}
}