int nfs41_sequence_done(struct rpc_task *task, struct nfs4_sequence_res *res)
{
	if (!nfs41_sequence_process(task, res))
		return 0;
	if (res->sr_slot != NULL)
		nfs41_sequence_free_slot(res);
	return 1;

}