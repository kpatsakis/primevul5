static bool fuse_writepage_add(struct fuse_writepage_args *new_wpa,
			       struct page *page)
{
	struct fuse_inode *fi = get_fuse_inode(new_wpa->inode);
	struct fuse_writepage_args *tmp;
	struct fuse_writepage_args *old_wpa;
	struct fuse_args_pages *new_ap = &new_wpa->ia.ap;

	WARN_ON(new_ap->num_pages != 0);
	new_ap->num_pages = 1;

	spin_lock(&fi->lock);
	old_wpa = fuse_insert_writeback(&fi->writepages, new_wpa);
	if (!old_wpa) {
		spin_unlock(&fi->lock);
		return true;
	}

	for (tmp = old_wpa->next; tmp; tmp = tmp->next) {
		pgoff_t curr_index;

		WARN_ON(tmp->inode != new_wpa->inode);
		curr_index = tmp->ia.write.in.offset >> PAGE_SHIFT;
		if (curr_index == page->index) {
			WARN_ON(tmp->ia.ap.num_pages != 1);
			swap(tmp->ia.ap.pages[0], new_ap->pages[0]);
			break;
		}
	}

	if (!tmp) {
		new_wpa->next = old_wpa->next;
		old_wpa->next = new_wpa;
	}

	spin_unlock(&fi->lock);

	if (tmp) {
		struct backing_dev_info *bdi = inode_to_bdi(new_wpa->inode);

		dec_wb_stat(&bdi->wb, WB_WRITEBACK);
		dec_node_page_state(new_ap->pages[0], NR_WRITEBACK_TEMP);
		wb_writeout_inc(&bdi->wb);
		fuse_writepage_free(new_wpa);
	}

	return false;
}