static int nfs4_open_recover_helper(struct nfs4_opendata *opendata,
		fmode_t fmode)
{
	struct nfs4_state *newstate;
	int ret;

	if (!nfs4_mode_match_open_stateid(opendata->state, fmode))
		return 0;
	opendata->o_arg.open_flags = 0;
	opendata->o_arg.fmode = fmode;
	opendata->o_arg.share_access = nfs4_map_atomic_open_share(
			NFS_SB(opendata->dentry->d_sb),
			fmode, 0);
	memset(&opendata->o_res, 0, sizeof(opendata->o_res));
	memset(&opendata->c_res, 0, sizeof(opendata->c_res));
	nfs4_init_opendata_res(opendata);
	ret = _nfs4_recover_proc_open(opendata);
	if (ret != 0)
		return ret; 
	newstate = nfs4_opendata_to_nfs4_state(opendata);
	if (IS_ERR(newstate))
		return PTR_ERR(newstate);
	if (newstate != opendata->state)
		ret = -ESTALE;
	nfs4_close_state(newstate, fmode);
	return ret;
}