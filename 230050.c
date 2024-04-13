nfs4_opendata_check_deleg(struct nfs4_opendata *data, struct nfs4_state *state)
{
	struct nfs_client *clp = NFS_SERVER(state->inode)->nfs_client;
	struct nfs_delegation *delegation;
	int delegation_flags = 0;

	rcu_read_lock();
	delegation = rcu_dereference(NFS_I(state->inode)->delegation);
	if (delegation)
		delegation_flags = delegation->flags;
	rcu_read_unlock();
	switch (data->o_arg.claim) {
	default:
		break;
	case NFS4_OPEN_CLAIM_DELEGATE_CUR:
	case NFS4_OPEN_CLAIM_DELEG_CUR_FH:
		pr_err_ratelimited("NFS: Broken NFSv4 server %s is "
				   "returning a delegation for "
				   "OPEN(CLAIM_DELEGATE_CUR)\n",
				   clp->cl_hostname);
		return;
	}
	if ((delegation_flags & 1UL<<NFS_DELEGATION_NEED_RECLAIM) == 0)
		nfs_inode_set_delegation(state->inode,
				data->owner->so_cred,
				data->o_res.delegation_type,
				&data->o_res.delegation,
				data->o_res.pagemod_limit);
	else
		nfs_inode_reclaim_delegation(state->inode,
				data->owner->so_cred,
				data->o_res.delegation_type,
				&data->o_res.delegation,
				data->o_res.pagemod_limit);

	if (data->o_res.do_recall)
		nfs_async_inode_return_delegation(state->inode,
						  &data->o_res.delegation);
}