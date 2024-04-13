static int madvise_free_single_vma(struct vm_area_struct *vma,
			unsigned long start_addr, unsigned long end_addr)
{
	struct mm_struct *mm = vma->vm_mm;
	struct mmu_notifier_range range;
	struct mmu_gather tlb;

	/* MADV_FREE works for only anon vma at the moment */
	if (!vma_is_anonymous(vma))
		return -EINVAL;

	range.start = max(vma->vm_start, start_addr);
	if (range.start >= vma->vm_end)
		return -EINVAL;
	range.end = min(vma->vm_end, end_addr);
	if (range.end <= vma->vm_start)
		return -EINVAL;
	mmu_notifier_range_init(&range, MMU_NOTIFY_CLEAR, 0, vma, mm,
				range.start, range.end);

	lru_add_drain();
	tlb_gather_mmu(&tlb, mm, range.start, range.end);
	update_hiwater_rss(mm);

	mmu_notifier_invalidate_range_start(&range);
	tlb_start_vma(&tlb, vma);
	walk_page_range(vma->vm_mm, range.start, range.end,
			&madvise_free_walk_ops, &tlb);
	tlb_end_vma(&tlb, vma);
	mmu_notifier_invalidate_range_end(&range);
	tlb_finish_mmu(&tlb, range.start, range.end);

	return 0;
}