static int rpc_lookup_req(struct nfs_priv *npriv, uint32_t prog, uint32_t ver)
{
	uint32_t data[16];
	struct packet *nfs_packet;
	uint32_t port;

	data[0] = 0; data[1] = 0;	/* auth credential */
	data[2] = 0; data[3] = 0;	/* auth verifier */
	data[4] = hton32(prog);
	data[5] = hton32(ver);
	data[6] = hton32(17);	/* IP_UDP */
	data[7] = 0;

	nfs_packet = rpc_req(npriv, PROG_PORTMAP, PORTMAP_GETPORT, data, 8);
	if (IS_ERR(nfs_packet))
		return PTR_ERR(nfs_packet);

	port = ntoh32(net_read_uint32(nfs_packet->data + sizeof(struct rpc_reply)));

	free(nfs_packet);

	return port;
}
