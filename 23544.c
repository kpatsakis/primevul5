static int do_linear_fault(struct mm_struct *mm, struct vm_area_struct *vma,
		unsigned long address, pte_t *page_table, pmd_t *pmd,
		int write_access, pte_t orig_pte)
{
	pgoff_t pgoff = (((address & PAGE_MASK)
			- vma->vm_start) >> PAGE_SHIFT) + vma->vm_pgoff;
	unsigned int flags = (write_access ? FAULT_FLAG_WRITE : 0);

	pte_unmap(page_table);
	return __do_fault(mm, vma, address, pmd, pgoff, flags, orig_pte);
}