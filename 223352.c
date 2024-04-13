vm_fault_t vmf_insert_pfn_pmd_prot(struct vm_fault *vmf, pfn_t pfn,
				   pgprot_t pgprot, bool write)
{
	unsigned long addr = vmf->address & PMD_MASK;
	struct vm_area_struct *vma = vmf->vma;
	pgtable_t pgtable = NULL;

	/*
	 * If we had pmd_special, we could avoid all these restrictions,
	 * but we need to be consistent with PTEs and architectures that
	 * can't support a 'special' bit.
	 */
	BUG_ON(!(vma->vm_flags & (VM_PFNMAP|VM_MIXEDMAP)) &&
			!pfn_t_devmap(pfn));
	BUG_ON((vma->vm_flags & (VM_PFNMAP|VM_MIXEDMAP)) ==
						(VM_PFNMAP|VM_MIXEDMAP));
	BUG_ON((vma->vm_flags & VM_PFNMAP) && is_cow_mapping(vma->vm_flags));

	if (addr < vma->vm_start || addr >= vma->vm_end)
		return VM_FAULT_SIGBUS;

	if (arch_needs_pgtable_deposit()) {
		pgtable = pte_alloc_one(vma->vm_mm);
		if (!pgtable)
			return VM_FAULT_OOM;
	}

	track_pfn_insert(vma, &pgprot, pfn);

	insert_pfn_pmd(vma, addr, vmf->pmd, pfn, pgprot, write, pgtable);
	return VM_FAULT_NOPAGE;
}