static long madvise_dontneed_free(struct vm_area_struct *vma,
				  struct vm_area_struct **prev,
				  unsigned long start, unsigned long end,
				  int behavior)
{
	*prev = vma;
	if (!can_madv_lru_vma(vma))
		return -EINVAL;

	if (!userfaultfd_remove(vma, start, end)) {
		*prev = NULL; /* mmap_sem has been dropped, prev is stale */

		down_read(&current->mm->mmap_sem);
		vma = find_vma(current->mm, start);
		if (!vma)
			return -ENOMEM;
		if (start < vma->vm_start) {
			/*
			 * This "vma" under revalidation is the one
			 * with the lowest vma->vm_start where start
			 * is also < vma->vm_end. If start <
			 * vma->vm_start it means an hole materialized
			 * in the user address space within the
			 * virtual range passed to MADV_DONTNEED
			 * or MADV_FREE.
			 */
			return -ENOMEM;
		}
		if (!can_madv_lru_vma(vma))
			return -EINVAL;
		if (end > vma->vm_end) {
			/*
			 * Don't fail if end > vma->vm_end. If the old
			 * vma was splitted while the mmap_sem was
			 * released the effect of the concurrent
			 * operation may not cause madvise() to
			 * have an undefined result. There may be an
			 * adjacent next vma that we'll walk
			 * next. userfaultfd_remove() will generate an
			 * UFFD_EVENT_REMOVE repetition on the
			 * end-vma->vm_end range, but the manager can
			 * handle a repetition fine.
			 */
			end = vma->vm_end;
		}
		VM_WARN_ON(start >= end);
	}

	if (behavior == MADV_DONTNEED)
		return madvise_dontneed_single_vma(vma, start, end);
	else if (behavior == MADV_FREE)
		return madvise_free_single_vma(vma, start, end);
	else
		return -EINVAL;
}