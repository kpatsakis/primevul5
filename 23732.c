static void fuse_writepage_finish(struct fuse_conn *fc, struct fuse_req *req)
{
	struct inode *inode = req->inode;
	struct fuse_inode *fi = get_fuse_inode(inode);
	struct backing_dev_info *bdi = inode->i_mapping->backing_dev_info;

	list_del(&req->writepages_entry);
	dec_bdi_stat(bdi, BDI_WRITEBACK);
	dec_zone_page_state(req->pages[0], NR_WRITEBACK_TEMP);
	bdi_writeout_inc(bdi);
	wake_up(&fi->page_waitq);
}