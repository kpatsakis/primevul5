static void fuse_readdir_cache_end(struct file *file, loff_t pos)
{
	struct fuse_inode *fi = get_fuse_inode(file_inode(file));
	loff_t end;

	spin_lock(&fi->rdc.lock);
	/* does cache end position match current position? */
	if (fi->rdc.pos != pos) {
		spin_unlock(&fi->rdc.lock);
		return;
	}

	fi->rdc.cached = true;
	end = ALIGN(fi->rdc.size, PAGE_SIZE);
	spin_unlock(&fi->rdc.lock);

	/* truncate unused tail of cache */
	truncate_inode_pages(file->f_mapping, end);
}