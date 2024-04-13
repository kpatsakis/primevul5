static int _nfs4_open_and_get_state(struct nfs4_opendata *opendata,
		int flags, struct nfs_open_context *ctx)
{
	struct nfs4_state_owner *sp = opendata->owner;
	struct nfs_server *server = sp->so_server;
	struct dentry *dentry;
	struct nfs4_state *state;
	fmode_t acc_mode = _nfs4_ctx_to_accessmode(ctx);
	struct inode *dir = d_inode(opendata->dir);
	unsigned long dir_verifier;
	unsigned int seq;
	int ret;

	seq = raw_seqcount_begin(&sp->so_reclaim_seqcount);
	dir_verifier = nfs_save_change_attribute(dir);

	ret = _nfs4_proc_open(opendata, ctx);
	if (ret != 0)
		goto out;

	state = _nfs4_opendata_to_nfs4_state(opendata);
	ret = PTR_ERR(state);
	if (IS_ERR(state))
		goto out;
	ctx->state = state;
	if (server->caps & NFS_CAP_POSIX_LOCK)
		set_bit(NFS_STATE_POSIX_LOCKS, &state->flags);
	if (opendata->o_res.rflags & NFS4_OPEN_RESULT_MAY_NOTIFY_LOCK)
		set_bit(NFS_STATE_MAY_NOTIFY_LOCK, &state->flags);

	dentry = opendata->dentry;
	if (d_really_is_negative(dentry)) {
		struct dentry *alias;
		d_drop(dentry);
		alias = d_exact_alias(dentry, state->inode);
		if (!alias)
			alias = d_splice_alias(igrab(state->inode), dentry);
		/* d_splice_alias() can't fail here - it's a non-directory */
		if (alias) {
			dput(ctx->dentry);
			ctx->dentry = dentry = alias;
		}
	}

	switch(opendata->o_arg.claim) {
	default:
		break;
	case NFS4_OPEN_CLAIM_NULL:
	case NFS4_OPEN_CLAIM_DELEGATE_CUR:
	case NFS4_OPEN_CLAIM_DELEGATE_PREV:
		if (!opendata->rpc_done)
			break;
		if (opendata->o_res.delegation_type != 0)
			dir_verifier = nfs_save_change_attribute(dir);
		nfs_set_verifier(dentry, dir_verifier);
	}

	/* Parse layoutget results before we check for access */
	pnfs_parse_lgopen(state->inode, opendata->lgp, ctx);

	ret = nfs4_opendata_access(sp->so_cred, opendata, state,
			acc_mode, flags);
	if (ret != 0)
		goto out;

	if (d_inode(dentry) == state->inode) {
		nfs_inode_attach_open_context(ctx);
		if (read_seqcount_retry(&sp->so_reclaim_seqcount, seq))
			nfs4_schedule_stateid_recovery(server, state);
	}

out:
	if (!opendata->cancelled)
		nfs4_sequence_free_slot(&opendata->o_res.seq_res);
	return ret;
}