static int fuse_readdir_cached(struct file *file, struct dir_context *ctx)
{
	struct fuse_file *ff = file->private_data;
	struct inode *inode = file_inode(file);
	struct fuse_conn *fc = get_fuse_conn(inode);
	struct fuse_inode *fi = get_fuse_inode(inode);
	enum fuse_parse_result res;
	pgoff_t index;
	unsigned int size;
	struct page *page;
	void *addr;

	/* Seeked?  If so, reset the cache stream */
	if (ff->readdir.pos != ctx->pos) {
		ff->readdir.pos = 0;
		ff->readdir.cache_off = 0;
	}

	/*
	 * We're just about to start reading into the cache or reading the
	 * cache; both cases require an up-to-date mtime value.
	 */
	if (!ctx->pos && fc->auto_inval_data) {
		int err = fuse_update_attributes(inode, file);

		if (err)
			return err;
	}

retry:
	spin_lock(&fi->rdc.lock);
retry_locked:
	if (!fi->rdc.cached) {
		/* Starting cache? Set cache mtime. */
		if (!ctx->pos && !fi->rdc.size) {
			fi->rdc.mtime = inode->i_mtime;
			fi->rdc.iversion = inode_query_iversion(inode);
		}
		spin_unlock(&fi->rdc.lock);
		return UNCACHED;
	}
	/*
	 * When at the beginning of the directory (i.e. just after opendir(3) or
	 * rewinddir(3)), then need to check whether directory contents have
	 * changed, and reset the cache if so.
	 */
	if (!ctx->pos) {
		if (inode_peek_iversion(inode) != fi->rdc.iversion ||
		    !timespec64_equal(&fi->rdc.mtime, &inode->i_mtime)) {
			fuse_rdc_reset(inode);
			goto retry_locked;
		}
	}

	/*
	 * If cache version changed since the last getdents() call, then reset
	 * the cache stream.
	 */
	if (ff->readdir.version != fi->rdc.version) {
		ff->readdir.pos = 0;
		ff->readdir.cache_off = 0;
	}
	/*
	 * If at the beginning of the cache, than reset version to
	 * current.
	 */
	if (ff->readdir.pos == 0)
		ff->readdir.version = fi->rdc.version;

	WARN_ON(fi->rdc.size < ff->readdir.cache_off);

	index = ff->readdir.cache_off >> PAGE_SHIFT;

	if (index == (fi->rdc.size >> PAGE_SHIFT))
		size = fi->rdc.size & ~PAGE_MASK;
	else
		size = PAGE_SIZE;
	spin_unlock(&fi->rdc.lock);

	/* EOF? */
	if ((ff->readdir.cache_off & ~PAGE_MASK) == size)
		return 0;

	page = find_get_page_flags(file->f_mapping, index,
				   FGP_ACCESSED | FGP_LOCK);
	spin_lock(&fi->rdc.lock);
	if (!page) {
		/*
		 * Uh-oh: page gone missing, cache is useless
		 */
		if (fi->rdc.version == ff->readdir.version)
			fuse_rdc_reset(inode);
		goto retry_locked;
	}

	/* Make sure it's still the same version after getting the page. */
	if (ff->readdir.version != fi->rdc.version) {
		spin_unlock(&fi->rdc.lock);
		unlock_page(page);
		put_page(page);
		goto retry;
	}
	spin_unlock(&fi->rdc.lock);

	/*
	 * Contents of the page are now protected against changing by holding
	 * the page lock.
	 */
	addr = kmap(page);
	res = fuse_parse_cache(ff, addr, size, ctx);
	kunmap(page);
	unlock_page(page);
	put_page(page);

	if (res == FOUND_ERR)
		return -EIO;

	if (res == FOUND_ALL)
		return 0;

	if (size == PAGE_SIZE) {
		/* We hit end of page: skip to next page. */
		ff->readdir.cache_off = ALIGN(ff->readdir.cache_off, PAGE_SIZE);
		goto retry;
	}

	/*
	 * End of cache reached.  If found position, then we are done, otherwise
	 * need to fall back to uncached, since the position we were looking for
	 * wasn't in the cache.
	 */
	return res == FOUND_SOME ? 0 : UNCACHED;
}