static int nfs_umountall_reply(unsigned char *pkt, unsigned len)
{
	int ret;

	ret = rpc_check_reply(pkt, 0);
	if (ret)
		return ret;

	memset(dirfh, 0, sizeof(dirfh));

	return 0;
}
