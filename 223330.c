static void touch_pmd(struct vm_area_struct *vma, unsigned long addr,
		pmd_t *pmd, int flags)
{
	pmd_t _pmd;

	_pmd = pmd_mkyoung(*pmd);
	if (flags & FOLL_WRITE)
		_pmd = pmd_mkdirty(_pmd);
	if (pmdp_set_access_flags(vma, addr & HPAGE_PMD_MASK,
				pmd, _pmd, flags & FOLL_WRITE))
		update_mmu_cache_pmd(vma, addr, pmd);
}