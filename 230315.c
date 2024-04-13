static void __nfs4_free_revoked_stateid(struct nfs_server *server,
		nfs4_stateid *stateid,
		const struct cred *cred)
{
	stateid->type = NFS4_REVOKED_STATEID_TYPE;
	nfs4_test_and_free_stateid(server, stateid, cred);
}