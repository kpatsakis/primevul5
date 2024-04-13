static int nfs4_do_open_expired(struct nfs_open_context *ctx, struct nfs4_state *state)
{
	struct nfs_server *server = NFS_SERVER(state->inode);
	struct nfs4_exception exception = { };
	int err;

	do {
		err = _nfs4_open_expired(ctx, state);
		trace_nfs4_open_expired(ctx, 0, err);
		if (nfs4_clear_cap_atomic_open_v1(server, err, &exception))
			continue;
		switch (err) {
		default:
			goto out;
		case -NFS4ERR_GRACE:
		case -NFS4ERR_DELAY:
			nfs4_handle_exception(server, err, &exception);
			err = 0;
		}
	} while (exception.retry);
out:
	return err;
}