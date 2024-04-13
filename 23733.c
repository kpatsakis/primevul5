__acquires(&fc->lock)
{
	struct fuse_inode *fi = get_fuse_inode(req->inode);
	loff_t size = i_size_read(req->inode);
	struct fuse_write_in *inarg = &req->misc.write.in;

	if (!fc->connected)
		goto out_free;

	if (inarg->offset + PAGE_CACHE_SIZE <= size) {
		inarg->size = PAGE_CACHE_SIZE;
	} else if (inarg->offset < size) {
		inarg->size = size & (PAGE_CACHE_SIZE - 1);
	} else {
		/* Got truncated off completely */
		goto out_free;
	}

	req->in.args[1].size = inarg->size;
	fi->writectr++;
	fuse_request_send_background_locked(fc, req);
	return;

 out_free:
	fuse_writepage_finish(fc, req);
	spin_unlock(&fc->lock);
	fuse_writepage_free(fc, req);
	fuse_put_request(fc, req);
	spin_lock(&fc->lock);
}