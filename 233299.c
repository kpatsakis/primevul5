static void xdr_buf_tail_copy_left(const struct xdr_buf *buf, unsigned int base,
				   unsigned int len, unsigned int shift)
{
	const struct kvec *tail = buf->tail;

	if (base >= tail->iov_len)
		return;
	if (len > tail->iov_len - base)
		len = tail->iov_len - base;
	/* Shift data into head */
	if (shift > buf->page_len + base) {
		const struct kvec *head = buf->head;
		unsigned int hdto =
			head->iov_len + buf->page_len + base - shift;
		unsigned int hdlen = len;

		if (WARN_ONCE(shift > head->iov_len + buf->page_len + base,
			      "SUNRPC: Misaligned data.\n"))
			return;
		if (hdto + hdlen > head->iov_len)
			hdlen = head->iov_len - hdto;
		memcpy(head->iov_base + hdto, tail->iov_base + base, hdlen);
		base += hdlen;
		len -= hdlen;
		if (!len)
			return;
	}
	/* Shift data into pages */
	if (shift > base) {
		unsigned int pgto = buf->page_len + base - shift;
		unsigned int pglen = len;

		if (pgto + pglen > buf->page_len)
			pglen = buf->page_len - pgto;
		_copy_to_pages(buf->pages, buf->page_base + pgto,
			       tail->iov_base + base, pglen);
		base += pglen;
		len -= pglen;
		if (!len)
			return;
	}
	memmove(tail->iov_base + base - shift, tail->iov_base + base, len);
}