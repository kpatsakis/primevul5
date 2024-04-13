static int fuse_file_mmap(struct file *file, struct vm_area_struct *vma)
{
	if ((vma->vm_flags & VM_SHARED) && (vma->vm_flags & VM_MAYWRITE)) {
		struct inode *inode = file->f_dentry->d_inode;
		struct fuse_conn *fc = get_fuse_conn(inode);
		struct fuse_inode *fi = get_fuse_inode(inode);
		struct fuse_file *ff = file->private_data;
		/*
		 * file may be written through mmap, so chain it onto the
		 * inodes's write_file list
		 */
		spin_lock(&fc->lock);
		if (list_empty(&ff->write_entry))
			list_add(&ff->write_entry, &fi->write_files);
		spin_unlock(&fc->lock);
	}
	file_accessed(file);
	vma->vm_ops = &fuse_file_vm_ops;
	return 0;
}