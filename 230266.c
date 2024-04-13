nfs4_init_uniquifier_client_string(struct nfs_client *clp)
{
	size_t len;
	char *str;

	len = 10 + 10 + 1 + 10 + 1 +
		strlen(nfs4_client_id_uniquifier) + 1 +
		strlen(clp->cl_rpcclient->cl_nodename) + 1;

	if (len > NFS4_OPAQUE_LIMIT + 1)
		return -EINVAL;

	/*
	 * Since this string is allocated at mount time, and held until the
	 * nfs_client is destroyed, we can use GFP_KERNEL here w/o worrying
	 * about a memory-reclaim deadlock.
	 */
	str = kmalloc(len, GFP_KERNEL);
	if (!str)
		return -ENOMEM;

	scnprintf(str, len, "Linux NFSv%u.%u %s/%s",
			clp->rpc_ops->version, clp->cl_minorversion,
			nfs4_client_id_uniquifier,
			clp->cl_rpcclient->cl_nodename);
	clp->cl_owner_id = str;
	return 0;
}