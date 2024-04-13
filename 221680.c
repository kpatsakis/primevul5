static void fuse_add_dirent_to_cache(struct file *file,
				     struct fuse_dirent *dirent, loff_t pos)
{
	struct fuse_inode *fi = get_fuse_inode(file_inode(file));
	size_t reclen = FUSE_DIRENT_SIZE(dirent);
	pgoff_t index;
	struct page *page;
	loff_t size;
	u64 version;
	unsigned int offset;
	void *addr;

	spin_lock(&fi->rdc.lock);
	/*
	 * Is cache already completed?  Or this entry does not go at the end of
	 * cache?
	 */
	if (fi->rdc.cached || pos != fi->rdc.pos) {
		spin_unlock(&fi->rdc.lock);
		return;
	}
	version = fi->rdc.version;
	size = fi->rdc.size;
	offset = size & ~PAGE_MASK;
	index = size >> PAGE_SHIFT;
	/* Dirent doesn't fit in current page?  Jump to next page. */
	if (offset + reclen > PAGE_SIZE) {
		index++;
		offset = 0;
	}
	spin_unlock(&fi->rdc.lock);

	if (offset) {
		page = find_lock_page(file->f_mapping, index);
	} else {
		page = find_or_create_page(file->f_mapping, index,
					   mapping_gfp_mask(file->f_mapping));
	}
	if (!page)
		return;

	spin_lock(&fi->rdc.lock);
	/* Raced with another readdir */
	if (fi->rdc.version != version || fi->rdc.size != size ||
	    WARN_ON(fi->rdc.pos != pos))
		goto unlock;

	addr = kmap_atomic(page);
	if (!offset)
		clear_page(addr);
	memcpy(addr + offset, dirent, reclen);
	kunmap_atomic(addr);
	fi->rdc.size = (index << PAGE_SHIFT) + offset + reclen;
	fi->rdc.pos = dirent->off;
unlock:
	spin_unlock(&fi->rdc.lock);
	unlock_page(page);
	put_page(page);
}