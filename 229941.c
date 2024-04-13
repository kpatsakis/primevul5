int nfs4_lock_delegation_recall(struct file_lock *fl, struct nfs4_state *state, const nfs4_stateid *stateid)
{
	struct nfs_server *server = NFS_SERVER(state->inode);
	int err;

	err = nfs4_set_lock_state(state, fl);
	if (err != 0)
		return err;
	err = _nfs4_do_setlk(state, F_SETLK, fl, NFS_LOCK_NEW);
	return nfs4_handle_delegation_recall_error(server, state, stateid, fl, err);
}