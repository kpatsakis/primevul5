void fuse_init_file_inode(struct inode *inode)
{
	struct fuse_inode *fi = get_fuse_inode(inode);

	inode->i_fop = &fuse_file_operations;
	inode->i_data.a_ops = &fuse_file_aops;

	INIT_LIST_HEAD(&fi->write_files);
	INIT_LIST_HEAD(&fi->queued_writes);
	fi->writectr = 0;
	init_waitqueue_head(&fi->page_waitq);
	fi->writepages = RB_ROOT;

	if (IS_ENABLED(CONFIG_FUSE_DAX))
		fuse_dax_inode_init(inode);
}