static int rpc_lookup_reply(int prog, unsigned char *pkt, unsigned len)
{
	uint32_t port;
	int ret;

	ret = rpc_check_reply(pkt, 0);
	if (ret)
		return ret;

	port = net_read_uint32((uint32_t *)(pkt + sizeof(struct rpc_reply)));
	switch (prog) {
	case PROG_MOUNT:
		nfs_server_mount_port = ntohl(port);
		break;
	case PROG_NFS:
		nfs_server_nfs_port = ntohl(port);
		break;
	}

	return 0;
}
