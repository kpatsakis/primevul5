nfs4_opendata_to_nfs4_state(struct nfs4_opendata *data)
{
	struct nfs4_state *ret;

	if (data->o_arg.claim == NFS4_OPEN_CLAIM_PREVIOUS)
		ret =_nfs4_opendata_reclaim_to_nfs4_state(data);
	else
		ret = _nfs4_opendata_to_nfs4_state(data);
	nfs4_sequence_free_slot(&data->o_res.seq_res);
	return ret;
}