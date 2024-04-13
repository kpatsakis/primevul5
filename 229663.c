static void nfs_state_log_update_open_stateid(struct nfs4_state *state)
{
	if (test_and_clear_bit(NFS_STATE_CHANGE_WAIT, &state->flags))
		wake_up_all(&state->waitq);
}