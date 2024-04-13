static int rpc_req(int rpc_prog, int rpc_proc, uint32_t *data, int datalen)
{
	struct rpc_call pkt;
	unsigned long id;
	int sport;
	int ret;
	unsigned char *payload = net_udp_get_payload(nfs_con);

	id = ++rpc_id;
	pkt.id = htonl(id);
	pkt.type = htonl(MSG_CALL);
	pkt.rpcvers = htonl(2);	/* use RPC version 2 */
	pkt.prog = htonl(rpc_prog);
	pkt.vers = htonl(2);	/* portmapper is version 2 */
	pkt.proc = htonl(rpc_proc);

	memcpy(payload, &pkt, sizeof(pkt));
	memcpy(payload + sizeof(pkt), data, datalen * sizeof(uint32_t));

	if (rpc_prog == PROG_PORTMAP)
		sport = SUNRPC_PORT;
	else if (rpc_prog == PROG_MOUNT)
		sport = nfs_server_mount_port;
	else
		sport = nfs_server_nfs_port;

	nfs_con->udp->uh_dport = htons(sport);
	ret = net_udp_send(nfs_con, sizeof(pkt) + datalen * sizeof(uint32_t));

	return ret;
}
