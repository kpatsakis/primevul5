nfs4_retry_setlk(struct nfs4_state *state, int cmd, struct file_lock *request)
{
	return nfs4_retry_setlk_simple(state, cmd, request);
}