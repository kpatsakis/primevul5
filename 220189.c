static void madvise_pageout_page_range(struct mmu_gather *tlb,
			     struct vm_area_struct *vma,
			     unsigned long addr, unsigned long end)
{
	struct madvise_walk_private walk_private = {
		.pageout = true,
		.tlb = tlb,
	};

	tlb_start_vma(tlb, vma);
	walk_page_range(vma->vm_mm, addr, end, &cold_walk_ops, &walk_private);
	tlb_end_vma(tlb, vma);
}