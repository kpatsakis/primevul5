static void xdr_realign_pages(struct xdr_stream *xdr)
{
	struct xdr_buf *buf = xdr->buf;
	struct kvec *iov = buf->head;
	unsigned int cur = xdr_stream_pos(xdr);
	unsigned int copied;

	/* Realign pages to current pointer position */
	if (iov->iov_len > cur) {
		copied = xdr_shrink_bufhead(buf, cur);
		trace_rpc_xdr_alignment(xdr, cur, copied);
		xdr_set_page(xdr, 0, buf->page_len);
	}
}