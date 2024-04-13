static inline struct nfs_inode *nfsi(struct inode *inode)
{
	return container_of(inode, struct nfs_inode, inode);
}
