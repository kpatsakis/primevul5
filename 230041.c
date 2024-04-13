static int nfs40_sequence_done(struct rpc_task *task,
			       struct nfs4_sequence_res *res)
{
	if (res->sr_slot != NULL)
		nfs40_sequence_free_slot(res);
	return 1;
}