spinlock_t *__pmd_trans_huge_lock(pmd_t *pmd, struct vm_area_struct *vma)
{
	spinlock_t *ptl;
	ptl = pmd_lock(vma->vm_mm, pmd);
	if (likely(is_swap_pmd(*pmd) || pmd_trans_huge(*pmd) ||
			pmd_devmap(*pmd)))
		return ptl;
	spin_unlock(ptl);
	return NULL;
}