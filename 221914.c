int fuse_fill_super_submount(struct super_block *sb,
			     struct fuse_inode *parent_fi)
{
	struct fuse_mount *fm = get_fuse_mount_super(sb);
	struct super_block *parent_sb = parent_fi->inode.i_sb;
	struct fuse_attr root_attr;
	struct inode *root;

	fuse_sb_defaults(sb);
	fm->sb = sb;

	WARN_ON(sb->s_bdi != &noop_backing_dev_info);
	sb->s_bdi = bdi_get(parent_sb->s_bdi);

	sb->s_xattr = parent_sb->s_xattr;
	sb->s_time_gran = parent_sb->s_time_gran;
	sb->s_blocksize = parent_sb->s_blocksize;
	sb->s_blocksize_bits = parent_sb->s_blocksize_bits;
	sb->s_subtype = kstrdup(parent_sb->s_subtype, GFP_KERNEL);
	if (parent_sb->s_subtype && !sb->s_subtype)
		return -ENOMEM;

	fuse_fill_attr_from_inode(&root_attr, parent_fi);
	root = fuse_iget(sb, parent_fi->nodeid, 0, &root_attr, 0, 0);
	/*
	 * This inode is just a duplicate, so it is not looked up and
	 * its nlookup should not be incremented.  fuse_iget() does
	 * that, though, so undo it here.
	 */
	get_fuse_inode(root)->nlookup--;
	sb->s_d_op = &fuse_dentry_operations;
	sb->s_root = d_make_root(root);
	if (!sb->s_root)
		return -ENOMEM;

	return 0;
}