static int nfs4_lookup_root(struct nfs_server *server, struct nfs_fh *fhandle,
		struct nfs_fsinfo *info)
{
	struct nfs4_exception exception = {
		.interruptible = true,
	};
	int err;
	do {
		err = _nfs4_lookup_root(server, fhandle, info);
		trace_nfs4_lookup_root(server, fhandle, info->fattr, err);
		switch (err) {
		case 0:
		case -NFS4ERR_WRONGSEC:
			goto out;
		default:
			err = nfs4_handle_exception(server, err, &exception);
		}
	} while (exception.retry);
out:
	return err;
}