nfs4_label_init_security(struct inode *dir, struct dentry *dentry,
	struct iattr *sattr, struct nfs4_label *label)
{
	int err;

	if (label == NULL)
		return NULL;

	if (nfs_server_capable(dir, NFS_CAP_SECURITY_LABEL) == 0)
		return NULL;

	err = security_dentry_init_security(dentry, sattr->ia_mode,
				&dentry->d_name, (void **)&label->label, &label->len);
	if (err == 0)
		return label;

	return NULL;
}