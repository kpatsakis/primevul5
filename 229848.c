static void nfs_resync_open_stateid_locked(struct nfs4_state *state)
{
	if (!(state->n_wronly || state->n_rdonly || state->n_rdwr))
		return;
	if (state->n_wronly)
		set_bit(NFS_O_WRONLY_STATE, &state->flags);
	if (state->n_rdonly)
		set_bit(NFS_O_RDONLY_STATE, &state->flags);
	if (state->n_rdwr)
		set_bit(NFS_O_RDWR_STATE, &state->flags);
	set_bit(NFS_OPEN_STATE, &state->flags);
}