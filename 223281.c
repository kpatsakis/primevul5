static void touch_pud(struct vm_area_struct *vma, unsigned long addr,
		pud_t *pud, int flags)
{
	pud_t _pud;

	_pud = pud_mkyoung(*pud);
	if (flags & FOLL_WRITE)
		_pud = pud_mkdirty(_pud);
	if (pudp_set_access_flags(vma, addr & HPAGE_PUD_MASK,
				pud, _pud, flags & FOLL_WRITE))
		update_mmu_cache_pud(vma, addr, pud);
}