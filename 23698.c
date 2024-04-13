static int fuse_page_mkwrite(struct vm_area_struct *vma, struct vm_fault *vmf)
{
	struct page *page = vmf->page;
	/*
	 * Don't use page->mapping as it may become NULL from a
	 * concurrent truncate.
	 */
	struct inode *inode = vma->vm_file->f_mapping->host;

	fuse_wait_on_page_writeback(inode, page->index);
	return 0;
}