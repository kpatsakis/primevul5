static int nfs40_test_and_free_expired_stateid(struct nfs_server *server,
		nfs4_stateid *stateid,
		const struct cred *cred)
{
	return -NFS4ERR_BAD_STATEID;
}