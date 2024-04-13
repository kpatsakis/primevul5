static void nfs_state_clear_open_state_flags(struct nfs4_state *state)
{
	clear_bit(NFS_O_RDWR_STATE, &state->flags);
	clear_bit(NFS_O_WRONLY_STATE, &state->flags);
	clear_bit(NFS_O_RDONLY_STATE, &state->flags);
	clear_bit(NFS_OPEN_STATE, &state->flags);
}