static struct fuse_writepage_args *fuse_find_writeback(struct fuse_inode *fi,
					    pgoff_t idx_from, pgoff_t idx_to)
{
	struct rb_node *n;

	n = fi->writepages.rb_node;

	while (n) {
		struct fuse_writepage_args *wpa;
		pgoff_t curr_index;

		wpa = rb_entry(n, struct fuse_writepage_args, writepages_entry);
		WARN_ON(get_fuse_inode(wpa->inode) != fi);
		curr_index = wpa->ia.write.in.offset >> PAGE_SHIFT;
		if (idx_from >= curr_index + wpa->ia.ap.num_pages)
			n = n->rb_right;
		else if (idx_to < curr_index)
			n = n->rb_left;
		else
			return wpa;
	}
	return NULL;
}