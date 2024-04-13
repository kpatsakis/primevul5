nfs4_set_security_label(struct inode *inode, const void *buf, size_t buflen)
{
	struct nfs4_label ilabel, *olabel = NULL;
	struct nfs_fattr fattr;
	int status;

	if (!nfs_server_capable(inode, NFS_CAP_SECURITY_LABEL))
		return -EOPNOTSUPP;

	nfs_fattr_init(&fattr);

	ilabel.pi = 0;
	ilabel.lfs = 0;
	ilabel.label = (char *)buf;
	ilabel.len = buflen;

	olabel = nfs4_label_alloc(NFS_SERVER(inode), GFP_KERNEL);
	if (IS_ERR(olabel)) {
		status = -PTR_ERR(olabel);
		goto out;
	}

	status = nfs4_do_set_security_label(inode, &ilabel, &fattr, olabel);
	if (status == 0)
		nfs_setsecurity(inode, &fattr, olabel);

	nfs4_label_free(olabel);
out:
	return status;
}