nfs41_free_lock_state(struct nfs_server *server, struct nfs4_lock_state *lsp)
{
	const struct cred *cred = lsp->ls_state->owner->so_cred;

	nfs41_free_stateid(server, &lsp->ls_stateid, cred, false);
	nfs4_free_lock_state(server, lsp);
}