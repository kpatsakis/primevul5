static noinline int do_no_pfn(struct mm_struct *mm, struct vm_area_struct *vma,
		     unsigned long address, pte_t *page_table, pmd_t *pmd,
		     int write_access)
{
	spinlock_t *ptl;
	pte_t entry;
	unsigned long pfn;

	pte_unmap(page_table);
	BUG_ON(!(vma->vm_flags & (VM_PFNMAP|VM_MIXEDMAP)));
	BUG_ON((vma->vm_flags & VM_PFNMAP) && is_cow_mapping(vma->vm_flags));

	pfn = vma->vm_ops->nopfn(vma, address & PAGE_MASK);

	BUG_ON((vma->vm_flags & VM_MIXEDMAP) && pfn_valid(pfn));

	if (unlikely(pfn == NOPFN_OOM))
		return VM_FAULT_OOM;
	else if (unlikely(pfn == NOPFN_SIGBUS))
		return VM_FAULT_SIGBUS;
	else if (unlikely(pfn == NOPFN_REFAULT))
		return 0;

	page_table = pte_offset_map_lock(mm, pmd, address, &ptl);

	/* Only go through if we didn't race with anybody else... */
	if (pte_none(*page_table)) {
		entry = pfn_pte(pfn, vma->vm_page_prot);
		if (write_access)
			entry = maybe_mkwrite(pte_mkdirty(entry), vma);
		set_pte_at(mm, address, page_table, entry);
	}
	pte_unmap_unlock(page_table, ptl);
	return 0;
}