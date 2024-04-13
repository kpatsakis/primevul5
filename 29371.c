static void nfs_set_fh(struct inode *inode, struct nfs_fh *fh)
{
	struct nfs_inode *ninode = nfsi(inode);

	ninode->fh = *fh;
}
