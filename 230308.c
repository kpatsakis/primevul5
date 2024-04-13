static int nfs4_commit_done(struct rpc_task *task, struct nfs_commit_data *data)
{
	if (!nfs4_sequence_done(task, &data->res.seq_res))
		return -EAGAIN;
	return data->commit_done_cb(task, data);
}