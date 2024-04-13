static void take_rmap_locks(struct vm_area_struct *vma)
{
	if (vma->vm_file)
		i_mmap_lock_write(vma->vm_file->f_mapping);
	if (vma->anon_vma)
		anon_vma_lock_write(vma->anon_vma);
}