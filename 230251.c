nfs4_listxattr_nfs4_user(struct inode *inode, char *list, size_t list_len)
{
	u64 cookie;
	bool eof;
	ssize_t ret, size;
	char *buf;
	size_t buflen;
	struct nfs_access_entry cache;

	if (!nfs_server_capable(inode, NFS_CAP_XATTR))
		return 0;

	if (!nfs_access_get_cached(inode, current_cred(), &cache, true)) {
		if (!(cache.mask & NFS_ACCESS_XALIST))
			return 0;
	}

	ret = nfs_revalidate_inode(NFS_SERVER(inode), inode);
	if (ret)
		return ret;

	ret = nfs4_xattr_cache_list(inode, list, list_len);
	if (ret >= 0 || (ret < 0 && ret != -ENOENT))
		return ret;

	cookie = 0;
	eof = false;
	buflen = list_len ? list_len : XATTR_LIST_MAX;
	buf = list_len ? list : NULL;
	size = 0;

	while (!eof) {
		ret = nfs42_proc_listxattrs(inode, buf, buflen,
		    &cookie, &eof);
		if (ret < 0)
			return ret;

		if (list_len) {
			buf += ret;
			buflen -= ret;
		}
		size += ret;
	}

	if (list_len)
		nfs4_xattr_cache_set_list(inode, list, size);

	return size;
}