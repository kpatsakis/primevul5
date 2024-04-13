int nfs4_sequence_done(struct rpc_task *task, struct nfs4_sequence_res *res)
{
	if (res->sr_slot == NULL)
		return 1;
	if (!res->sr_slot->table->session)
		return nfs40_sequence_done(task, res);
	return nfs41_sequence_done(task, res);
}