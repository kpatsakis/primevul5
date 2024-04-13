_shift_data_left_pages(struct page **pages, size_t pgto_base,
			size_t pgfrom_base, size_t len)
{
	struct page **pgfrom, **pgto;
	char *vfrom, *vto;
	size_t copy;

	BUG_ON(pgfrom_base <= pgto_base);

	if (!len)
		return;

	pgto = pages + (pgto_base >> PAGE_SHIFT);
	pgfrom = pages + (pgfrom_base >> PAGE_SHIFT);

	pgto_base &= ~PAGE_MASK;
	pgfrom_base &= ~PAGE_MASK;

	do {
		if (pgto_base >= PAGE_SIZE) {
			pgto_base = 0;
			pgto++;
		}
		if (pgfrom_base >= PAGE_SIZE){
			pgfrom_base = 0;
			pgfrom++;
		}

		copy = len;
		if (copy > (PAGE_SIZE - pgto_base))
			copy = PAGE_SIZE - pgto_base;
		if (copy > (PAGE_SIZE - pgfrom_base))
			copy = PAGE_SIZE - pgfrom_base;

		vto = kmap_atomic(*pgto);
		if (*pgto != *pgfrom) {
			vfrom = kmap_atomic(*pgfrom);
			memcpy(vto + pgto_base, vfrom + pgfrom_base, copy);
			kunmap_atomic(vfrom);
		} else
			memmove(vto + pgto_base, vto + pgfrom_base, copy);
		flush_dcache_page(*pgto);
		kunmap_atomic(vto);

		pgto_base += copy;
		pgfrom_base += copy;

	} while ((len -= copy) != 0);
}