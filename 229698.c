nfs4_init_callback_netid(const struct nfs_client *clp, char *buf, size_t len)
{
	if (strchr(clp->cl_ipaddr, ':') != NULL)
		return scnprintf(buf, len, "tcp6");
	else
		return scnprintf(buf, len, "tcp");
}