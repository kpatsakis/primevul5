void xdr_truncate_encode(struct xdr_stream *xdr, size_t len)
{
	struct xdr_buf *buf = xdr->buf;
	struct kvec *head = buf->head;
	struct kvec *tail = buf->tail;
	int fraglen;
	int new;

	if (len > buf->len) {
		WARN_ON_ONCE(1);
		return;
	}
	xdr_commit_encode(xdr);

	fraglen = min_t(int, buf->len - len, tail->iov_len);
	tail->iov_len -= fraglen;
	buf->len -= fraglen;
	if (tail->iov_len) {
		xdr->p = tail->iov_base + tail->iov_len;
		WARN_ON_ONCE(!xdr->end);
		WARN_ON_ONCE(!xdr->iov);
		return;
	}
	WARN_ON_ONCE(fraglen);
	fraglen = min_t(int, buf->len - len, buf->page_len);
	buf->page_len -= fraglen;
	buf->len -= fraglen;

	new = buf->page_base + buf->page_len;

	xdr->page_ptr = buf->pages + (new >> PAGE_SHIFT);

	if (buf->page_len) {
		xdr->p = page_address(*xdr->page_ptr);
		xdr->end = (void *)xdr->p + PAGE_SIZE;
		xdr->p = (void *)xdr->p + (new % PAGE_SIZE);
		WARN_ON_ONCE(xdr->iov);
		return;
	}
	if (fraglen)
		xdr->end = head->iov_base + head->iov_len;
	/* (otherwise assume xdr->end is already set) */
	xdr->page_ptr--;
	head->iov_len = len;
	buf->len = len;
	xdr->p = head->iov_base + head->iov_len;
	xdr->iov = buf->head;
}