static int nfs4_proc_unlink_done(struct rpc_task *task, struct inode *dir)
{
	struct nfs_unlinkdata *data = task->tk_calldata;
	struct nfs_removeres *res = &data->res;

	if (!nfs4_sequence_done(task, &res->seq_res))
		return 0;
	if (nfs4_async_handle_error(task, res->server, NULL,
				    &data->timeout) == -EAGAIN)
		return 0;
	if (task->tk_status == 0)
		nfs4_update_changeattr(dir, &res->cinfo,
				res->dir_attr->time_start,
				NFS_INO_INVALID_DATA);
	return 1;
}