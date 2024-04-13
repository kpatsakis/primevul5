static void rpc_lookup_req(int prog, int ver)
{
	uint32_t data[16];

	data[0] = 0; data[1] = 0;	/* auth credential */
	data[2] = 0; data[3] = 0;	/* auth verifier */
	data[4] = htonl(prog);
	data[5] = htonl(ver);
	data[6] = htonl(17);	/* IP_UDP */
	data[7] = 0;

	rpc_req(PROG_PORTMAP, PORTMAP_GETPORT, data, 8);
}
