static void xdr_buf_head_copy_right(const struct xdr_buf *buf,
				    unsigned int base, unsigned int len,
				    unsigned int shift)
{
	const struct kvec *head = buf->head;
	const struct kvec *tail = buf->tail;
	unsigned int to = base + shift;
	unsigned int pglen = 0, pgto = 0;
	unsigned int talen = 0, tato = 0;

	if (base >= head->iov_len)
		return;
	if (len > head->iov_len - base)
		len = head->iov_len - base;
	if (to >= buf->page_len + head->iov_len) {
		tato = to - buf->page_len - head->iov_len;
		talen = len;
	} else if (to >= head->iov_len) {
		pgto = to - head->iov_len;
		pglen = len;
		if (pgto + pglen > buf->page_len) {
			talen = pgto + pglen - buf->page_len;
			pglen -= talen;
		}
	} else {
		pglen = len - to;
		if (pglen > buf->page_len) {
			talen = pglen - buf->page_len;
			pglen = buf->page_len;
		}
	}

	len -= talen;
	base += len;
	if (talen + tato > tail->iov_len)
		talen = tail->iov_len > tato ? tail->iov_len - tato : 0;
	memcpy(tail->iov_base + tato, head->iov_base + base, talen);

	len -= pglen;
	base -= pglen;
	_copy_to_pages(buf->pages, buf->page_base + pgto, head->iov_base + base,
		       pglen);

	base -= len;
	memmove(head->iov_base + to, head->iov_base + base, len);
}