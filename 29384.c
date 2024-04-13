static int nfs_mount_reply(unsigned char *pkt, unsigned len)
{
	int ret;

	ret = rpc_check_reply(pkt, 1);
	if (ret)
		return ret;

	memcpy(dirfh, pkt + sizeof(struct rpc_reply) + 4, NFS_FHSIZE);

	return 0;
}
