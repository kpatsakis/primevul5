static void insert_pfn_pud(struct vm_area_struct *vma, unsigned long addr,
		pud_t *pud, pfn_t pfn, pgprot_t prot, bool write)
{
	struct mm_struct *mm = vma->vm_mm;
	pud_t entry;
	spinlock_t *ptl;

	ptl = pud_lock(mm, pud);
	if (!pud_none(*pud)) {
		if (write) {
			if (pud_pfn(*pud) != pfn_t_to_pfn(pfn)) {
				WARN_ON_ONCE(!is_huge_zero_pud(*pud));
				goto out_unlock;
			}
			entry = pud_mkyoung(*pud);
			entry = maybe_pud_mkwrite(pud_mkdirty(entry), vma);
			if (pudp_set_access_flags(vma, addr, pud, entry, 1))
				update_mmu_cache_pud(vma, addr, pud);
		}
		goto out_unlock;
	}

	entry = pud_mkhuge(pfn_t_pud(pfn, prot));
	if (pfn_t_devmap(pfn))
		entry = pud_mkdevmap(entry);
	if (write) {
		entry = pud_mkyoung(pud_mkdirty(entry));
		entry = maybe_pud_mkwrite(entry, vma);
	}
	set_pud_at(mm, addr, pud, entry);
	update_mmu_cache_pud(vma, addr, pud);

out_unlock:
	spin_unlock(ptl);
}