static bool nfs_open_stateid_recover_openmode(struct nfs4_state *state)
{
	if (state->n_rdonly && !test_bit(NFS_O_RDONLY_STATE, &state->flags))
		return true;
	if (state->n_wronly && !test_bit(NFS_O_WRONLY_STATE, &state->flags))
		return true;
	if (state->n_rdwr && !test_bit(NFS_O_RDWR_STATE, &state->flags))
		return true;
	return false;
}