void fuse_init_common(struct inode *inode)
{
	inode->i_op = &fuse_common_inode_operations;
}