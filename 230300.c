static int nfs4_proc_rename_done(struct rpc_task *task, struct inode *old_dir,
				 struct inode *new_dir)
{
	struct nfs_renamedata *data = task->tk_calldata;
	struct nfs_renameres *res = &data->res;

	if (!nfs4_sequence_done(task, &res->seq_res))
		return 0;
	if (nfs4_async_handle_error(task, res->server, NULL, &data->timeout) == -EAGAIN)
		return 0;

	if (task->tk_status == 0) {
		if (new_dir != old_dir) {
			/* Note: If we moved a directory, nlink will change */
			nfs4_update_changeattr(old_dir, &res->old_cinfo,
					res->old_fattr->time_start,
					NFS_INO_INVALID_OTHER |
					    NFS_INO_INVALID_DATA);
			nfs4_update_changeattr(new_dir, &res->new_cinfo,
					res->new_fattr->time_start,
					NFS_INO_INVALID_OTHER |
					    NFS_INO_INVALID_DATA);
		} else
			nfs4_update_changeattr(old_dir, &res->old_cinfo,
					res->old_fattr->time_start,
					NFS_INO_INVALID_DATA);
	}
	return 1;
}