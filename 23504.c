static inline unsigned long zap_pmd_range(struct mmu_gather *tlb,
				struct vm_area_struct *vma, pud_t *pud,
				unsigned long addr, unsigned long end,
				long *zap_work, struct zap_details *details)
{
	pmd_t *pmd;
	unsigned long next;

	pmd = pmd_offset(pud, addr);
	do {
		next = pmd_addr_end(addr, end);
		if (pmd_none_or_clear_bad(pmd)) {
			(*zap_work)--;
			continue;
		}
		next = zap_pte_range(tlb, vma, pmd, addr, next,
						zap_work, details);
	} while (pmd++, addr = next, (addr != end && *zap_work > 0));

	return addr;
}