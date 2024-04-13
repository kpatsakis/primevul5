static void xdr_buf_pages_copy_right(const struct xdr_buf *buf,
				     unsigned int base, unsigned int len,
				     unsigned int shift)
{
	const struct kvec *tail = buf->tail;
	unsigned int to = base + shift;
	unsigned int pglen = 0;
	unsigned int talen = 0, tato = 0;

	if (base >= buf->page_len)
		return;
	if (len > buf->page_len - base)
		len = buf->page_len - base;
	if (to >= buf->page_len) {
		tato = to - buf->page_len;
		if (tail->iov_len >= len + tato)
			talen = len;
		else if (tail->iov_len > tato)
			talen = tail->iov_len - tato;
	} else if (len + to >= buf->page_len) {
		pglen = buf->page_len - to;
		talen = len - pglen;
		if (talen > tail->iov_len)
			talen = tail->iov_len;
	} else
		pglen = len;

	_copy_from_pages(tail->iov_base + tato, buf->pages,
			 buf->page_base + base + pglen, talen);
	_shift_data_right_pages(buf->pages, buf->page_base + to,
				buf->page_base + base, pglen);
}