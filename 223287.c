void __split_huge_pud(struct vm_area_struct *vma, pud_t *pud,
		unsigned long address)
{
	spinlock_t *ptl;
	struct mmu_notifier_range range;

	mmu_notifier_range_init(&range, MMU_NOTIFY_CLEAR, 0, vma, vma->vm_mm,
				address & HPAGE_PUD_MASK,
				(address & HPAGE_PUD_MASK) + HPAGE_PUD_SIZE);
	mmu_notifier_invalidate_range_start(&range);
	ptl = pud_lock(vma->vm_mm, pud);
	if (unlikely(!pud_trans_huge(*pud) && !pud_devmap(*pud)))
		goto out;
	__split_huge_pud_locked(vma, pud, range.start);

out:
	spin_unlock(ptl);
	/*
	 * No need to double call mmu_notifier->invalidate_range() callback as
	 * the above pudp_huge_clear_flush_notify() did already call it.
	 */
	mmu_notifier_invalidate_range_only_end(&range);
}