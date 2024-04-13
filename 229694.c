static int nfs4_do_fsinfo(struct nfs_server *server, struct nfs_fh *fhandle, struct nfs_fsinfo *fsinfo)
{
	struct nfs4_exception exception = {
		.interruptible = true,
	};
	int err;

	do {
		err = _nfs4_do_fsinfo(server, fhandle, fsinfo);
		trace_nfs4_fsinfo(server, fhandle, fsinfo->fattr, err);
		if (err == 0) {
			nfs4_set_lease_period(server->nfs_client, fsinfo->lease_time * HZ);
			break;
		}
		err = nfs4_handle_exception(server, err, &exception);
	} while (exception.retry);
	return err;
}