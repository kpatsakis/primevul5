static const char *nfs_get_link(struct dentry *dentry, struct inode *inode)
{
	struct nfs_inode *ninode = nfsi(inode);
	struct nfs_priv *npriv = ninode->npriv;
	int ret;

	ret = nfs_readlink_req(npriv, &ninode->fh, &inode->i_link);
	if (ret)
		return ERR_PTR(ret);

	return inode->i_link;
}
