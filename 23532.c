struct page *vm_normal_page(struct vm_area_struct *vma, unsigned long addr,
				pte_t pte)
{
	unsigned long pfn;

	if (HAVE_PTE_SPECIAL) {
		if (likely(!pte_special(pte))) {
			VM_BUG_ON(!pfn_valid(pte_pfn(pte)));
			return pte_page(pte);
		}
		VM_BUG_ON(!(vma->vm_flags & (VM_PFNMAP | VM_MIXEDMAP)));
		return NULL;
	}

	/* !HAVE_PTE_SPECIAL case follows: */

	pfn = pte_pfn(pte);

	if (unlikely(vma->vm_flags & (VM_PFNMAP|VM_MIXEDMAP))) {
		if (vma->vm_flags & VM_MIXEDMAP) {
			if (!pfn_valid(pfn))
				return NULL;
			goto out;
		} else {
			unsigned long off;
			off = (addr - vma->vm_start) >> PAGE_SHIFT;
			if (pfn == vma->vm_pgoff + off)
				return NULL;
			if (!is_cow_mapping(vma->vm_flags))
				return NULL;
		}
	}

	VM_BUG_ON(!pfn_valid(pfn));

	/*
	 * NOTE! We still have PageReserved() pages in the page tables.
	 *
	 * eg. VDSO mappings can cause them to exist.
	 */
out:
	return pfn_to_page(pfn);
}