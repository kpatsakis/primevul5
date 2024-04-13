static long madvise_pageout(struct vm_area_struct *vma,
			struct vm_area_struct **prev,
			unsigned long start_addr, unsigned long end_addr)
{
	struct mm_struct *mm = vma->vm_mm;
	struct mmu_gather tlb;

	*prev = vma;
	if (!can_madv_lru_vma(vma))
		return -EINVAL;

	if (!can_do_pageout(vma))
		return 0;

	lru_add_drain();
	tlb_gather_mmu(&tlb, mm, start_addr, end_addr);
	madvise_pageout_page_range(&tlb, vma, start_addr, end_addr);
	tlb_finish_mmu(&tlb, start_addr, end_addr);

	return 0;
}