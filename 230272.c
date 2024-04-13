static void nfs4_handle_delay_or_session_error(struct nfs_server *server,
		int err, struct nfs4_exception *exception)
{
	exception->retry = 0;
	switch(err) {
	case -NFS4ERR_DELAY:
	case -NFS4ERR_RETRY_UNCACHED_REP:
		nfs4_handle_exception(server, err, exception);
		break;
	case -NFS4ERR_BADSESSION:
	case -NFS4ERR_BADSLOT:
	case -NFS4ERR_BAD_HIGH_SLOT:
	case -NFS4ERR_CONN_NOT_BOUND_TO_SESSION:
	case -NFS4ERR_DEADSESSION:
		nfs4_do_handle_exception(server, err, exception);
	}
}