static void xdr_buf_pages_zero(const struct xdr_buf *buf, unsigned int pgbase,
			       unsigned int len)
{
	struct page **pages = buf->pages;
	struct page **page;
	char *vpage;
	unsigned int zero;

	if (!len)
		return;
	if (pgbase >= buf->page_len) {
		xdr_buf_iov_zero(buf->tail, pgbase - buf->page_len, len);
		return;
	}
	if (pgbase + len > buf->page_len) {
		xdr_buf_iov_zero(buf->tail, 0, pgbase + len - buf->page_len);
		len = buf->page_len - pgbase;
	}

	pgbase += buf->page_base;

	page = pages + (pgbase >> PAGE_SHIFT);
	pgbase &= ~PAGE_MASK;

	do {
		zero = PAGE_SIZE - pgbase;
		if (zero > len)
			zero = len;

		vpage = kmap_atomic(*page);
		memset(vpage + pgbase, 0, zero);
		kunmap_atomic(vpage);

		flush_dcache_page(*page);
		pgbase = 0;
		page++;

	} while ((len -= zero) != 0);
}