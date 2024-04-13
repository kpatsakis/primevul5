void huge_pud_set_accessed(struct vm_fault *vmf, pud_t orig_pud)
{
	pud_t entry;
	unsigned long haddr;
	bool write = vmf->flags & FAULT_FLAG_WRITE;

	vmf->ptl = pud_lock(vmf->vma->vm_mm, vmf->pud);
	if (unlikely(!pud_same(*vmf->pud, orig_pud)))
		goto unlock;

	entry = pud_mkyoung(orig_pud);
	if (write)
		entry = pud_mkdirty(entry);
	haddr = vmf->address & HPAGE_PUD_MASK;
	if (pudp_set_access_flags(vmf->vma, haddr, vmf->pud, entry, write))
		update_mmu_cache_pud(vmf->vma, vmf->address, vmf->pud);

unlock:
	spin_unlock(vmf->ptl);
}