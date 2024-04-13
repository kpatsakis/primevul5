int vm_insert_mixed(struct vm_area_struct *vma, unsigned long addr,
			unsigned long pfn)
{
	BUG_ON(!(vma->vm_flags & VM_MIXEDMAP));

	if (addr < vma->vm_start || addr >= vma->vm_end)
		return -EFAULT;

	/*
	 * If we don't have pte special, then we have to use the pfn_valid()
	 * based VM_MIXEDMAP scheme (see vm_normal_page), and thus we *must*
	 * refcount the page if pfn_valid is true (hence insert_page rather
	 * than insert_pfn).
	 */
	if (!HAVE_PTE_SPECIAL && pfn_valid(pfn)) {
		struct page *page;

		page = pfn_to_page(pfn);
		return insert_page(vma, addr, page, vma->vm_page_prot);
	}
	return insert_pfn(vma, addr, pfn, vma->vm_page_prot);
}