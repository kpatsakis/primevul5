static int nfs_init_inode(struct nfs_priv *npriv, struct inode *inode,
			  unsigned int mode)
{
	struct nfs_inode *ninode = nfsi(inode);

	ninode->npriv = npriv;

	inode->i_ino = get_next_ino();
	inode->i_mode = mode;

	switch (inode->i_mode & S_IFMT) {
	default:
		return -EINVAL;
	case S_IFREG:
		inode->i_op = &nfs_file_inode_operations;
		inode->i_fop = &nfs_file_operations;
		break;
	case S_IFDIR:
		inode->i_op = &nfs_dir_inode_operations;
		inode->i_fop = &nfs_dir_operations;
		inc_nlink(inode);
		break;
	case S_IFLNK:
		inode->i_op = &nfs_symlink_inode_operations;
		break;
	}

	return 0;
}
