static inline void set_pte_at(struct mm_struct *mm, unsigned long addr,
			      pte_t *ptep, pte_t pte)
{
	if (pte_valid_ng(pte)) {
		if (!pte_special(pte) && pte_exec(pte))
			__sync_icache_dcache(pte, addr);
		if (pte_dirty(pte) && pte_write(pte))
			pte_val(pte) &= ~PTE_RDONLY;
		else
			pte_val(pte) |= PTE_RDONLY;
	}

	set_pte(ptep, pte);
}