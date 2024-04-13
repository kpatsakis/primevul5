static struct page *new_page(struct page *page, unsigned long start)
{
	struct vm_area_struct *vma;
	unsigned long uninitialized_var(address);

	vma = find_vma(current->mm, start);
	while (vma) {
		address = page_address_in_vma(page, vma);
		if (address != -EFAULT)
			break;
		vma = vma->vm_next;
	}

	if (PageHuge(page)) {
		return alloc_huge_page_vma(page_hstate(compound_head(page)),
				vma, address);
	} else if (PageTransHuge(page)) {
		struct page *thp;

		thp = alloc_hugepage_vma(GFP_TRANSHUGE, vma, address,
					 HPAGE_PMD_ORDER);
		if (!thp)
			return NULL;
		prep_transhuge_page(thp);
		return thp;
	}
	/*
	 * if !vma, alloc_page_vma() will use task or system default policy
	 */
	return alloc_page_vma(GFP_HIGHUSER_MOVABLE | __GFP_RETRY_MAYFAIL,
			vma, address);
}