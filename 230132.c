static int nfs4_proc_destroy_clientid(struct nfs_client *clp,
		const struct cred *cred)
{
	unsigned int loop;
	int ret;

	for (loop = NFS4_MAX_LOOP_ON_RECOVER; loop != 0; loop--) {
		ret = _nfs4_proc_destroy_clientid(clp, cred);
		switch (ret) {
		case -NFS4ERR_DELAY:
		case -NFS4ERR_CLIENTID_BUSY:
			ssleep(1);
			break;
		default:
			return ret;
		}
	}
	return 0;
}