static bool xdr_set_next_buffer(struct xdr_stream *xdr)
{
	if (xdr->page_ptr != NULL)
		xdr_set_next_page(xdr);
	else if (xdr->iov == xdr->buf->head)
		xdr_set_page(xdr, 0, xdr_stream_remaining(xdr));
	return xdr->p != xdr->end;
}