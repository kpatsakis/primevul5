static int nfs4_do_create(struct inode *dir, struct dentry *dentry, struct nfs4_createdata *data)
{
	int status = nfs4_call_sync(NFS_SERVER(dir)->client, NFS_SERVER(dir), &data->msg,
				    &data->arg.seq_args, &data->res.seq_res, 1);
	if (status == 0) {
		spin_lock(&dir->i_lock);
		nfs4_update_changeattr_locked(dir, &data->res.dir_cinfo,
				data->res.fattr->time_start,
				NFS_INO_INVALID_DATA);
		/* Creating a directory bumps nlink in the parent */
		if (data->arg.ftype == NF4DIR)
			nfs4_inc_nlink_locked(dir);
		spin_unlock(&dir->i_lock);
		status = nfs_instantiate(dentry, data->res.fh, data->res.fattr, data->res.label);
	}
	return status;
}