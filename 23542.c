static inline unsigned long zap_pud_range(struct mmu_gather *tlb,
				struct vm_area_struct *vma, pgd_t *pgd,
				unsigned long addr, unsigned long end,
				long *zap_work, struct zap_details *details)
{
	pud_t *pud;
	unsigned long next;

	pud = pud_offset(pgd, addr);
	do {
		next = pud_addr_end(addr, end);
		if (pud_none_or_clear_bad(pud)) {
			(*zap_work)--;
			continue;
		}
		next = zap_pmd_range(tlb, vma, pud, addr, next,
						zap_work, details);
	} while (pud++, addr = next, (addr != end && *zap_work > 0));

	return addr;
}