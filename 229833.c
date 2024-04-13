static int nfs4_xattr_get_nfs4_user(const struct xattr_handler *handler,
				    struct dentry *unused, struct inode *inode,
				    const char *key, void *buf, size_t buflen)
{
	struct nfs_access_entry cache;
	ssize_t ret;

	if (!nfs_server_capable(inode, NFS_CAP_XATTR))
		return -EOPNOTSUPP;

	if (!nfs_access_get_cached(inode, current_cred(), &cache, true)) {
		if (!(cache.mask & NFS_ACCESS_XAREAD))
			return -EACCES;
	}

	ret = nfs_revalidate_inode(NFS_SERVER(inode), inode);
	if (ret)
		return ret;

	ret = nfs4_xattr_cache_get(inode, key, buf, buflen);
	if (ret >= 0 || (ret < 0 && ret != -ENOENT))
		return ret;

	ret = nfs42_proc_getxattr(inode, key, buf, buflen);

	return ret;
}