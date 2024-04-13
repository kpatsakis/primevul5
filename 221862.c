struct inode *fuse_ilookup(struct fuse_conn *fc, u64 nodeid,
			   struct fuse_mount **fm)
{
	struct fuse_mount *fm_iter;
	struct inode *inode;

	WARN_ON(!rwsem_is_locked(&fc->killsb));
	list_for_each_entry(fm_iter, &fc->mounts, fc_entry) {
		if (!fm_iter->sb)
			continue;

		inode = ilookup5(fm_iter->sb, nodeid, fuse_inode_eq, &nodeid);
		if (inode) {
			if (fm)
				*fm = fm_iter;
			return inode;
		}
	}

	return NULL;
}