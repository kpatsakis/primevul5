static int _nfs4_recover_proc_open(struct nfs4_opendata *data)
{
	struct inode *dir = d_inode(data->dir);
	struct nfs_openres *o_res = &data->o_res;
	int status;

	status = nfs4_run_open_task(data, NULL);
	if (status != 0 || !data->rpc_done)
		return status;

	nfs_fattr_map_and_free_names(NFS_SERVER(dir), &data->f_attr);

	if (o_res->rflags & NFS4_OPEN_RESULT_CONFIRM)
		status = _nfs4_proc_open_confirm(data);

	return status;
}