static pte_t move_soft_dirty_pte(pte_t pte)
{
	/*
	 * Set soft dirty bit so we can notice
	 * in userspace the ptes were moved.
	 */
#ifdef CONFIG_MEM_SOFT_DIRTY
	if (pte_present(pte))
		pte = pte_mksoft_dirty(pte);
	else if (is_swap_pte(pte))
		pte = pte_swp_mksoft_dirty(pte);
#endif
	return pte;
}