static int queue_pages_test_walk(unsigned long start, unsigned long end,
				struct mm_walk *walk)
{
	struct vm_area_struct *vma = walk->vma;
	struct queue_pages *qp = walk->private;
	unsigned long endvma = vma->vm_end;
	unsigned long flags = qp->flags;

	/* range check first */
	VM_BUG_ON_VMA((vma->vm_start > start) || (vma->vm_end < end), vma);

	if (!qp->first) {
		qp->first = vma;
		if (!(flags & MPOL_MF_DISCONTIG_OK) &&
			(qp->start < vma->vm_start))
			/* hole at head side of range */
			return -EFAULT;
	}
	if (!(flags & MPOL_MF_DISCONTIG_OK) &&
		((vma->vm_end < qp->end) &&
		(!vma->vm_next || vma->vm_end < vma->vm_next->vm_start)))
		/* hole at middle or tail of range */
		return -EFAULT;

	/*
	 * Need check MPOL_MF_STRICT to return -EIO if possible
	 * regardless of vma_migratable
	 */
	if (!vma_migratable(vma) &&
	    !(flags & MPOL_MF_STRICT))
		return 1;

	if (endvma > end)
		endvma = end;

	if (flags & MPOL_MF_LAZY) {
		/* Similar to task_numa_work, skip inaccessible VMAs */
		if (!is_vm_hugetlb_page(vma) &&
			(vma->vm_flags & (VM_READ | VM_EXEC | VM_WRITE)) &&
			!(vma->vm_flags & VM_MIXEDMAP))
			change_prot_numa(vma, start, endvma);
		return 1;
	}

	/* queue pages from current vma */
	if (flags & MPOL_MF_VALID)
		return 0;
	return 1;
}