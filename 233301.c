void xdr_init_decode_pages(struct xdr_stream *xdr, struct xdr_buf *buf,
			   struct page **pages, unsigned int len)
{
	memset(buf, 0, sizeof(*buf));
	buf->pages =  pages;
	buf->page_len =  len;
	buf->buflen =  len;
	buf->len = len;
	xdr_init_decode(xdr, buf, NULL, NULL);
}