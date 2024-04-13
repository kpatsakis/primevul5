pte_t *get_locked_pte(struct mm_struct *mm, unsigned long addr,
			spinlock_t **ptl)
{
	pgd_t * pgd = pgd_offset(mm, addr);
	pud_t * pud = pud_alloc(mm, pgd, addr);
	if (pud) {
		pmd_t * pmd = pmd_alloc(mm, pud, addr);
		if (pmd)
			return pte_alloc_map_lock(mm, pmd, addr, ptl);
	}
	return NULL;
}