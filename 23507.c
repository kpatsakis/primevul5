unsigned long unmap_vmas(struct mmu_gather **tlbp,
		struct vm_area_struct *vma, unsigned long start_addr,
		unsigned long end_addr, unsigned long *nr_accounted,
		struct zap_details *details)
{
	long zap_work = ZAP_BLOCK_SIZE;
	unsigned long tlb_start = 0;	/* For tlb_finish_mmu */
	int tlb_start_valid = 0;
	unsigned long start = start_addr;
	spinlock_t *i_mmap_lock = details? details->i_mmap_lock: NULL;
	int fullmm = (*tlbp)->fullmm;

	for ( ; vma && vma->vm_start < end_addr; vma = vma->vm_next) {
		unsigned long end;

		start = max(vma->vm_start, start_addr);
		if (start >= vma->vm_end)
			continue;
		end = min(vma->vm_end, end_addr);
		if (end <= vma->vm_start)
			continue;

		if (vma->vm_flags & VM_ACCOUNT)
			*nr_accounted += (end - start) >> PAGE_SHIFT;

		while (start != end) {
			if (!tlb_start_valid) {
				tlb_start = start;
				tlb_start_valid = 1;
			}

			if (unlikely(is_vm_hugetlb_page(vma))) {
				unmap_hugepage_range(vma, start, end);
				zap_work -= (end - start) /
						(HPAGE_SIZE / PAGE_SIZE);
				start = end;
			} else
				start = unmap_page_range(*tlbp, vma,
						start, end, &zap_work, details);

			if (zap_work > 0) {
				BUG_ON(start != end);
				break;
			}

			tlb_finish_mmu(*tlbp, tlb_start, start);

			if (need_resched() ||
				(i_mmap_lock && spin_needbreak(i_mmap_lock))) {
				if (i_mmap_lock) {
					*tlbp = NULL;
					goto out;
				}
				cond_resched();
			}

			*tlbp = tlb_gather_mmu(vma->vm_mm, fullmm);
			tlb_start_valid = 0;
			zap_work = ZAP_BLOCK_SIZE;
		}
	}
out:
	return start;	/* which is now the end (or restart) address */
}