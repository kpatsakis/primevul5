static int swapin_walk_pmd_entry(pmd_t *pmd, unsigned long start,
	unsigned long end, struct mm_walk *walk)
{
	pte_t *orig_pte;
	struct vm_area_struct *vma = walk->private;
	unsigned long index;

	if (pmd_none_or_trans_huge_or_clear_bad(pmd))
		return 0;

	for (index = start; index != end; index += PAGE_SIZE) {
		pte_t pte;
		swp_entry_t entry;
		struct page *page;
		spinlock_t *ptl;

		orig_pte = pte_offset_map_lock(vma->vm_mm, pmd, start, &ptl);
		pte = *(orig_pte + ((index - start) / PAGE_SIZE));
		pte_unmap_unlock(orig_pte, ptl);

		if (pte_present(pte) || pte_none(pte))
			continue;
		entry = pte_to_swp_entry(pte);
		if (unlikely(non_swap_entry(entry)))
			continue;

		page = read_swap_cache_async(entry, GFP_HIGHUSER_MOVABLE,
							vma, index, false);
		if (page)
			put_page(page);
	}

	return 0;
}