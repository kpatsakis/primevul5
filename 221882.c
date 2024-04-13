static void fuse_writepage_end(struct fuse_mount *fm, struct fuse_args *args,
			       int error)
{
	struct fuse_writepage_args *wpa =
		container_of(args, typeof(*wpa), ia.ap.args);
	struct inode *inode = wpa->inode;
	struct fuse_inode *fi = get_fuse_inode(inode);

	mapping_set_error(inode->i_mapping, error);
	spin_lock(&fi->lock);
	rb_erase(&wpa->writepages_entry, &fi->writepages);
	while (wpa->next) {
		struct fuse_mount *fm = get_fuse_mount(inode);
		struct fuse_write_in *inarg = &wpa->ia.write.in;
		struct fuse_writepage_args *next = wpa->next;

		wpa->next = next->next;
		next->next = NULL;
		next->ia.ff = fuse_file_get(wpa->ia.ff);
		tree_insert(&fi->writepages, next);

		/*
		 * Skip fuse_flush_writepages() to make it easy to crop requests
		 * based on primary request size.
		 *
		 * 1st case (trivial): there are no concurrent activities using
		 * fuse_set/release_nowrite.  Then we're on safe side because
		 * fuse_flush_writepages() would call fuse_send_writepage()
		 * anyway.
		 *
		 * 2nd case: someone called fuse_set_nowrite and it is waiting
		 * now for completion of all in-flight requests.  This happens
		 * rarely and no more than once per page, so this should be
		 * okay.
		 *
		 * 3rd case: someone (e.g. fuse_do_setattr()) is in the middle
		 * of fuse_set_nowrite..fuse_release_nowrite section.  The fact
		 * that fuse_set_nowrite returned implies that all in-flight
		 * requests were completed along with all of their secondary
		 * requests.  Further primary requests are blocked by negative
		 * writectr.  Hence there cannot be any in-flight requests and
		 * no invocations of fuse_writepage_end() while we're in
		 * fuse_set_nowrite..fuse_release_nowrite section.
		 */
		fuse_send_writepage(fm, next, inarg->offset + inarg->size);
	}
	fi->writectr--;
	fuse_writepage_finish(fm, wpa);
	spin_unlock(&fi->lock);
	fuse_writepage_free(wpa);
}