static int nfs40_open_expired(struct nfs4_state_owner *sp, struct nfs4_state *state)
{
	/* NFSv4.0 doesn't allow for delegation recovery on open expire */
	nfs40_clear_delegation_stateid(state);
	nfs_state_clear_open_state_flags(state);
	return nfs4_open_expired(sp, state);
}