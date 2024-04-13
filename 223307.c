void huge_pmd_set_accessed(struct vm_fault *vmf, pmd_t orig_pmd)
{
	pmd_t entry;
	unsigned long haddr;
	bool write = vmf->flags & FAULT_FLAG_WRITE;

	vmf->ptl = pmd_lock(vmf->vma->vm_mm, vmf->pmd);
	if (unlikely(!pmd_same(*vmf->pmd, orig_pmd)))
		goto unlock;

	entry = pmd_mkyoung(orig_pmd);
	if (write)
		entry = pmd_mkdirty(entry);
	haddr = vmf->address & HPAGE_PMD_MASK;
	if (pmdp_set_access_flags(vmf->vma, haddr, vmf->pmd, entry, write))
		update_mmu_cache_pmd(vmf->vma, vmf->address, vmf->pmd);

unlock:
	spin_unlock(vmf->ptl);
}