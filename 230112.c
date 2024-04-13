_nfs4_opendata_to_nfs4_state(struct nfs4_opendata *data)
{
	struct nfs4_state *state;

	if (!data->rpc_done) {
		state = nfs4_try_open_cached(data);
		trace_nfs4_cached_open(data->state);
		goto out;
	}

	state = nfs4_opendata_find_nfs4_state(data);
	if (IS_ERR(state))
		goto out;

	if (data->o_res.delegation_type != 0)
		nfs4_opendata_check_deleg(data, state);
	if (!update_open_stateid(state, &data->o_res.stateid,
				NULL, data->o_arg.fmode)) {
		nfs4_put_open_state(state);
		state = ERR_PTR(-EAGAIN);
	}
out:
	nfs_release_seqid(data->o_arg.seqid);
	return state;
}