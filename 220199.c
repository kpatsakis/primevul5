static long madvise_remove(struct vm_area_struct *vma,
				struct vm_area_struct **prev,
				unsigned long start, unsigned long end)
{
	loff_t offset;
	int error;
	struct file *f;

	*prev = NULL;	/* tell sys_madvise we drop mmap_sem */

	if (vma->vm_flags & VM_LOCKED)
		return -EINVAL;

	f = vma->vm_file;

	if (!f || !f->f_mapping || !f->f_mapping->host) {
			return -EINVAL;
	}

	if ((vma->vm_flags & (VM_SHARED|VM_WRITE)) != (VM_SHARED|VM_WRITE))
		return -EACCES;

	offset = (loff_t)(start - vma->vm_start)
			+ ((loff_t)vma->vm_pgoff << PAGE_SHIFT);

	/*
	 * Filesystem's fallocate may need to take i_mutex.  We need to
	 * explicitly grab a reference because the vma (and hence the
	 * vma's reference to the file) can go away as soon as we drop
	 * mmap_sem.
	 */
	get_file(f);
	if (userfaultfd_remove(vma, start, end)) {
		/* mmap_sem was not released by userfaultfd_remove() */
		up_read(&current->mm->mmap_sem);
	}
	error = vfs_fallocate(f,
				FALLOC_FL_PUNCH_HOLE | FALLOC_FL_KEEP_SIZE,
				offset, end - start);
	fput(f);
	down_read(&current->mm->mmap_sem);
	return error;
}