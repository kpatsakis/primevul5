static void fuse_fill_attr_from_inode(struct fuse_attr *attr,
				      const struct fuse_inode *fi)
{
	*attr = (struct fuse_attr){
		.ino		= fi->inode.i_ino,
		.size		= fi->inode.i_size,
		.blocks		= fi->inode.i_blocks,
		.atime		= fi->inode.i_atime.tv_sec,
		.mtime		= fi->inode.i_mtime.tv_sec,
		.ctime		= fi->inode.i_ctime.tv_sec,
		.atimensec	= fi->inode.i_atime.tv_nsec,
		.mtimensec	= fi->inode.i_mtime.tv_nsec,
		.ctimensec	= fi->inode.i_ctime.tv_nsec,
		.mode		= fi->inode.i_mode,
		.nlink		= fi->inode.i_nlink,
		.uid		= fi->inode.i_uid.val,
		.gid		= fi->inode.i_gid.val,
		.rdev		= fi->inode.i_rdev,
		.blksize	= 1u << fi->inode.i_blkbits,
	};
}