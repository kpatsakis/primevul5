static int nfs41_test_and_free_expired_stateid(struct nfs_server *server,
		nfs4_stateid *stateid,
		const struct cred *cred)
{
	int status;

	switch (stateid->type) {
	default:
		break;
	case NFS4_INVALID_STATEID_TYPE:
	case NFS4_SPECIAL_STATEID_TYPE:
		return -NFS4ERR_BAD_STATEID;
	case NFS4_REVOKED_STATEID_TYPE:
		goto out_free;
	}

	status = nfs41_test_stateid(server, stateid, cred);
	switch (status) {
	case -NFS4ERR_EXPIRED:
	case -NFS4ERR_ADMIN_REVOKED:
	case -NFS4ERR_DELEG_REVOKED:
		break;
	default:
		return status;
	}
out_free:
	/* Ack the revoked state to the server */
	nfs41_free_stateid(server, stateid, cred, true);
	return -NFS4ERR_EXPIRED;
}