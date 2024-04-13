static void fuse_rdc_reset(struct inode *inode)
{
	struct fuse_inode *fi = get_fuse_inode(inode);

	fi->rdc.cached = false;
	fi->rdc.version++;
	fi->rdc.size = 0;
	fi->rdc.pos = 0;
}