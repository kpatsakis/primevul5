int copy_huge_pud(struct mm_struct *dst_mm, struct mm_struct *src_mm,
		  pud_t *dst_pud, pud_t *src_pud, unsigned long addr,
		  struct vm_area_struct *vma)
{
	spinlock_t *dst_ptl, *src_ptl;
	pud_t pud;
	int ret;

	dst_ptl = pud_lock(dst_mm, dst_pud);
	src_ptl = pud_lockptr(src_mm, src_pud);
	spin_lock_nested(src_ptl, SINGLE_DEPTH_NESTING);

	ret = -EAGAIN;
	pud = *src_pud;
	if (unlikely(!pud_trans_huge(pud) && !pud_devmap(pud)))
		goto out_unlock;

	/*
	 * When page table lock is held, the huge zero pud should not be
	 * under splitting since we don't split the page itself, only pud to
	 * a page table.
	 */
	if (is_huge_zero_pud(pud)) {
		/* No huge zero pud yet */
	}

	pudp_set_wrprotect(src_mm, addr, src_pud);
	pud = pud_mkold(pud_wrprotect(pud));
	set_pud_at(dst_mm, addr, dst_pud, pud);

	ret = 0;
out_unlock:
	spin_unlock(src_ptl);
	spin_unlock(dst_ptl);
	return ret;
}