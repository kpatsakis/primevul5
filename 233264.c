_copy_to_pages(struct page **pages, size_t pgbase, const char *p, size_t len)
{
	struct page **pgto;
	char *vto;
	size_t copy;

	if (!len)
		return;

	pgto = pages + (pgbase >> PAGE_SHIFT);
	pgbase &= ~PAGE_MASK;

	for (;;) {
		copy = PAGE_SIZE - pgbase;
		if (copy > len)
			copy = len;

		vto = kmap_atomic(*pgto);
		memcpy(vto + pgbase, p, copy);
		kunmap_atomic(vto);

		len -= copy;
		if (len == 0)
			break;

		pgbase += copy;
		if (pgbase == PAGE_SIZE) {
			flush_dcache_page(*pgto);
			pgbase = 0;
			pgto++;
		}
		p += copy;
	}
	flush_dcache_page(*pgto);
}