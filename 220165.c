static bool move_normal_pmd(struct vm_area_struct *vma, unsigned long old_addr,
		  unsigned long new_addr, unsigned long old_end,
		  pmd_t *old_pmd, pmd_t *new_pmd)
{
	spinlock_t *old_ptl, *new_ptl;
	struct mm_struct *mm = vma->vm_mm;
	pmd_t pmd;

	if ((old_addr & ~PMD_MASK) || (new_addr & ~PMD_MASK)
	    || old_end - old_addr < PMD_SIZE)
		return false;

	/*
	 * The destination pmd shouldn't be established, free_pgtables()
	 * should have release it.
	 */
	if (WARN_ON(!pmd_none(*new_pmd)))
		return false;

	/*
	 * We don't have to worry about the ordering of src and dst
	 * ptlocks because exclusive mmap_sem prevents deadlock.
	 */
	old_ptl = pmd_lock(vma->vm_mm, old_pmd);
	new_ptl = pmd_lockptr(mm, new_pmd);
	if (new_ptl != old_ptl)
		spin_lock_nested(new_ptl, SINGLE_DEPTH_NESTING);

	/* Clear the pmd */
	pmd = *old_pmd;
	pmd_clear(old_pmd);

	VM_BUG_ON(!pmd_none(*new_pmd));

	/* Set the new pmd */
	set_pmd_at(mm, new_addr, new_pmd, pmd);
	flush_tlb_range(vma, old_addr, old_addr + PMD_SIZE);
	if (new_ptl != old_ptl)
		spin_unlock(new_ptl);
	spin_unlock(old_ptl);

	return true;
}