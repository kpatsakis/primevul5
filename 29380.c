static struct packet *rpc_req(struct nfs_priv *npriv, int rpc_prog,
			      int rpc_proc, uint32_t *data, int datalen)
{
	struct rpc_call pkt;
	unsigned short dport;
	int ret;
	unsigned char *payload = net_udp_get_payload(npriv->con);
	int nfserr;
	int tries = 0;

	npriv->rpc_id++;

	pkt.id = hton32(npriv->rpc_id);
	pkt.type = hton32(MSG_CALL);
	pkt.rpcvers = hton32(2);	/* use RPC version 2 */
	pkt.prog = hton32(rpc_prog);
	pkt.proc = hton32(rpc_proc);

	debug("%s: prog: %d, proc: %d\n", __func__, rpc_prog, rpc_proc);

	if (rpc_prog == PROG_PORTMAP) {
		dport = SUNRPC_PORT;
		pkt.vers = hton32(2);
	} else if (rpc_prog == PROG_MOUNT) {
		dport = npriv->mount_port;
		pkt.vers = hton32(3);
	} else {
		dport = npriv->nfs_port;
		pkt.vers = hton32(3);
	}

	memcpy(payload, &pkt, sizeof(pkt));
	memcpy(payload + sizeof(pkt), data, datalen * sizeof(uint32_t));

	npriv->con->udp->uh_dport = hton16(dport);

	nfs_timer_start = get_time_ns();

again:
	ret = net_udp_send(npriv->con,
			sizeof(pkt) + datalen * sizeof(uint32_t));
	if (ret) {
		if (is_timeout(nfs_timer_start, NFS_TIMEOUT)) {
			tries++;
			if (tries == NFS_MAX_RESEND)
				return ERR_PTR(-ETIMEDOUT);
		}

		goto again;
	}

	nfs_timer_start = get_time_ns();

	nfs_state = STATE_START;

	while (nfs_state != STATE_DONE) {
		if (ctrlc())
			return ERR_PTR(-EINTR);

		net_poll();

		if (is_timeout(nfs_timer_start, NFS_TIMEOUT)) {
			tries++;
			if (tries == NFS_MAX_RESEND)
				return ERR_PTR(-ETIMEDOUT);
			goto again;
		}

		ret = rpc_check_reply(npriv->nfs_packet, rpc_prog,
				      npriv->rpc_id, &nfserr);
		if (!ret) {
			if (rpc_prog == PROG_NFS && nfserr) {
				free(npriv->nfs_packet);
				return ERR_PTR(nfserr);
			} else {
				return npriv->nfs_packet;
			}
		}
	}

	return npriv->nfs_packet;
}
