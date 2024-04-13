static ssize_t nfs4_listxattr(struct dentry *dentry, char *list, size_t size)
{
	ssize_t error, error2, error3;

	error = generic_listxattr(dentry, list, size);
	if (error < 0)
		return error;
	if (list) {
		list += error;
		size -= error;
	}

	error2 = nfs4_listxattr_nfs4_label(d_inode(dentry), list, size);
	if (error2 < 0)
		return error2;

	if (list) {
		list += error2;
		size -= error2;
	}

	error3 = nfs4_listxattr_nfs4_user(d_inode(dentry), list, size);
	if (error3 < 0)
		return error3;

	return error + error2 + error3;
}