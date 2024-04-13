static void nfs4_free_revoked_stateid(struct nfs_server *server,
		const nfs4_stateid *stateid,
		const struct cred *cred)
{
	nfs4_stateid tmp;

	nfs4_stateid_copy(&tmp, stateid);
	__nfs4_free_revoked_stateid(server, &tmp, cred);
}