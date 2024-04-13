spinlock_t *__pud_trans_huge_lock(pud_t *pud, struct vm_area_struct *vma)
{
	spinlock_t *ptl;

	ptl = pud_lock(vma->vm_mm, pud);
	if (likely(pud_trans_huge(*pud) || pud_devmap(*pud)))
		return ptl;
	spin_unlock(ptl);
	return NULL;
}