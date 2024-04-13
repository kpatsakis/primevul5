static void xdr_buf_pages_copy_left(const struct xdr_buf *buf,
				    unsigned int base, unsigned int len,
				    unsigned int shift)
{
	unsigned int pgto;

	if (base >= buf->page_len)
		return;
	if (len > buf->page_len - base)
		len = buf->page_len - base;
	/* Shift data into head */
	if (shift > base) {
		const struct kvec *head = buf->head;
		unsigned int hdto = head->iov_len + base - shift;
		unsigned int hdlen = len;

		if (WARN_ONCE(shift > head->iov_len + base,
			      "SUNRPC: Misaligned data.\n"))
			return;
		if (hdto + hdlen > head->iov_len)
			hdlen = head->iov_len - hdto;
		_copy_from_pages(head->iov_base + hdto, buf->pages,
				 buf->page_base + base, hdlen);
		base += hdlen;
		len -= hdlen;
		if (!len)
			return;
	}
	pgto = base - shift;
	_shift_data_left_pages(buf->pages, buf->page_base + pgto,
			       buf->page_base + base, len);
}