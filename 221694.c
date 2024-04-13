static int fuse_file_mmap(struct file *file, struct vm_area_struct *vma)
{
	struct fuse_file *ff = file->private_data;

	/* DAX mmap is superior to direct_io mmap */
	if (FUSE_IS_DAX(file_inode(file)))
		return fuse_dax_mmap(file, vma);

	if (ff->open_flags & FOPEN_DIRECT_IO) {
		/* Can't provide the coherency needed for MAP_SHARED */
		if (vma->vm_flags & VM_MAYSHARE)
			return -ENODEV;

		invalidate_inode_pages2(file->f_mapping);

		return generic_file_mmap(file, vma);
	}

	if ((vma->vm_flags & VM_SHARED) && (vma->vm_flags & VM_MAYWRITE))
		fuse_link_write_file(file);

	file_accessed(file);
	vma->vm_ops = &fuse_file_vm_ops;
	return 0;
}