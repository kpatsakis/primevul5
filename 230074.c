static int nfs41_test_stateid(struct nfs_server *server,
		nfs4_stateid *stateid,
		const struct cred *cred)
{
	struct nfs4_exception exception = {
		.interruptible = true,
	};
	int err;
	do {
		err = _nfs41_test_stateid(server, stateid, cred);
		nfs4_handle_delay_or_session_error(server, err, &exception);
	} while (exception.retry);
	return err;
}