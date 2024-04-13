int nfs4_destroy_clientid(struct nfs_client *clp)
{
	const struct cred *cred;
	int ret = 0;

	if (clp->cl_mvops->minor_version < 1)
		goto out;
	if (clp->cl_exchange_flags == 0)
		goto out;
	if (clp->cl_preserve_clid)
		goto out;
	cred = nfs4_get_clid_cred(clp);
	ret = nfs4_proc_destroy_clientid(clp, cred);
	put_cred(cred);
	switch (ret) {
	case 0:
	case -NFS4ERR_STALE_CLIENTID:
		clp->cl_exchange_flags = 0;
	}
out:
	return ret;
}