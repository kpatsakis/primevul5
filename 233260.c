static unsigned int xdr_align_pages(struct xdr_stream *xdr, unsigned int len)
{
	struct xdr_buf *buf = xdr->buf;
	unsigned int nwords = XDR_QUADLEN(len);
	unsigned int copied;

	if (xdr->nwords == 0)
		return 0;

	xdr_realign_pages(xdr);
	if (nwords > xdr->nwords) {
		nwords = xdr->nwords;
		len = nwords << 2;
	}
	if (buf->page_len <= len)
		len = buf->page_len;
	else if (nwords < xdr->nwords) {
		/* Truncate page data and move it into the tail */
		copied = xdr_shrink_pagelen(buf, len);
		trace_rpc_xdr_alignment(xdr, len, copied);
	}
	return len;
}