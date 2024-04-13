static struct fuse_writepage_args *fuse_insert_writeback(struct rb_root *root,
						struct fuse_writepage_args *wpa)
{
	pgoff_t idx_from = wpa->ia.write.in.offset >> PAGE_SHIFT;
	pgoff_t idx_to = idx_from + wpa->ia.ap.num_pages - 1;
	struct rb_node **p = &root->rb_node;
	struct rb_node  *parent = NULL;

	WARN_ON(!wpa->ia.ap.num_pages);
	while (*p) {
		struct fuse_writepage_args *curr;
		pgoff_t curr_index;

		parent = *p;
		curr = rb_entry(parent, struct fuse_writepage_args,
				writepages_entry);
		WARN_ON(curr->inode != wpa->inode);
		curr_index = curr->ia.write.in.offset >> PAGE_SHIFT;

		if (idx_from >= curr_index + curr->ia.ap.num_pages)
			p = &(*p)->rb_right;
		else if (idx_to < curr_index)
			p = &(*p)->rb_left;
		else
			return curr;
	}

	rb_link_node(&wpa->writepages_entry, parent, p);
	rb_insert_color(&wpa->writepages_entry, root);
	return NULL;
}