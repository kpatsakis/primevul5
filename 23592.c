static int apply_to_pte_range(struct mm_struct *mm, pmd_t *pmd,
				     unsigned long addr, unsigned long end,
				     pte_fn_t fn, void *data)
{
	pte_t *pte;
	int err;
	pgtable_t token;
	spinlock_t *uninitialized_var(ptl);

	pte = (mm == &init_mm) ?
		pte_alloc_kernel(pmd, addr) :
		pte_alloc_map_lock(mm, pmd, addr, &ptl);
	if (!pte)
		return -ENOMEM;

	BUG_ON(pmd_huge(*pmd));

	token = pmd_pgtable(*pmd);

	do {
		err = fn(pte, token, addr, data);
		if (err)
			break;
	} while (pte++, addr += PAGE_SIZE, addr != end);

	if (mm != &init_mm)
		pte_unmap_unlock(pte-1, ptl);
	return err;
}