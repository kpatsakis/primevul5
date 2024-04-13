nfs4_listxattr_nfs4_label(struct inode *inode, char *list, size_t list_len)
{
	int len = 0;

	if (nfs_server_capable(inode, NFS_CAP_SECURITY_LABEL)) {
		len = security_inode_listsecurity(inode, list, list_len);
		if (list_len && len > list_len)
			return -ERANGE;
	}
	return len;
}