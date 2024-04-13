static void nfs_finish_clear_delegation_stateid(struct nfs4_state *state,
		const nfs4_stateid *stateid)
{
	nfs_remove_bad_delegation(state->inode, stateid);
	nfs_state_clear_delegation(state);
}