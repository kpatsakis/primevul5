static int nfs4_xattr_set_nfs4_user(const struct xattr_handler *handler,
				    struct dentry *unused, struct inode *inode,
				    const char *key, const void *buf,
				    size_t buflen, int flags)
{
	struct nfs_access_entry cache;
	int ret;

	if (!nfs_server_capable(inode, NFS_CAP_XATTR))
		return -EOPNOTSUPP;

	/*
	 * There is no mapping from the MAY_* flags to the NFS_ACCESS_XA*
	 * flags right now. Handling of xattr operations use the normal
	 * file read/write permissions.
	 *
	 * Just in case the server has other ideas (which RFC 8276 allows),
	 * do a cached access check for the XA* flags to possibly avoid
	 * doing an RPC and getting EACCES back.
	 */
	if (!nfs_access_get_cached(inode, current_cred(), &cache, true)) {
		if (!(cache.mask & NFS_ACCESS_XAWRITE))
			return -EACCES;
	}

	if (buf == NULL) {
		ret = nfs42_proc_removexattr(inode, key);
		if (!ret)
			nfs4_xattr_cache_remove(inode, key);
	} else {
		ret = nfs42_proc_setxattr(inode, key, buf, buflen, flags);
		if (!ret)
			nfs4_xattr_cache_add(inode, key, buf, NULL, buflen);
	}

	return ret;
}