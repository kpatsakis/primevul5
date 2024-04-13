void xdr_write_pages(struct xdr_stream *xdr, struct page **pages, unsigned int base,
		 unsigned int len)
{
	struct xdr_buf *buf = xdr->buf;
	struct kvec *iov = buf->tail;
	buf->pages = pages;
	buf->page_base = base;
	buf->page_len = len;

	iov->iov_base = (char *)xdr->p;
	iov->iov_len  = 0;
	xdr->iov = iov;

	if (len & 3) {
		unsigned int pad = 4 - (len & 3);

		BUG_ON(xdr->p >= xdr->end);
		iov->iov_base = (char *)xdr->p + (len & 3);
		iov->iov_len  += pad;
		len += pad;
		*xdr->p++ = 0;
	}
	buf->buflen += len;
	buf->len += len;
}