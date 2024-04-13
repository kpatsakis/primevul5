static int sctp_send_asconf_add_ip(struct sock		*sk,
				   struct sockaddr	*addrs,
				   int 			addrcnt)
{
	struct sctp_sock		*sp;
	struct sctp_endpoint		*ep;
	struct sctp_association		*asoc;
	struct sctp_bind_addr		*bp;
	struct sctp_chunk		*chunk;
	struct sctp_sockaddr_entry	*laddr;
	union sctp_addr			*addr;
	union sctp_addr			saveaddr;
	void				*addr_buf;
	struct sctp_af			*af;
	struct list_head		*p;
	int 				i;
	int 				retval = 0;

	sp = sctp_sk(sk);
	ep = sp->ep;

	if (!ep->asconf_enable)
		return retval;

	pr_debug("%s: sk:%p, addrs:%p, addrcnt:%d\n",
		 __func__, sk, addrs, addrcnt);

	list_for_each_entry(asoc, &ep->asocs, asocs) {
		if (!asoc->peer.asconf_capable)
			continue;

		if (asoc->peer.addip_disabled_mask & SCTP_PARAM_ADD_IP)
			continue;

		if (!sctp_state(asoc, ESTABLISHED))
			continue;

		/* Check if any address in the packed array of addresses is
		 * in the bind address list of the association. If so,
		 * do not send the asconf chunk to its peer, but continue with
		 * other associations.
		 */
		addr_buf = addrs;
		for (i = 0; i < addrcnt; i++) {
			addr = addr_buf;
			af = sctp_get_af_specific(addr->v4.sin_family);
			if (!af) {
				retval = -EINVAL;
				goto out;
			}

			if (sctp_assoc_lookup_laddr(asoc, addr))
				break;

			addr_buf += af->sockaddr_len;
		}
		if (i < addrcnt)
			continue;

		/* Use the first valid address in bind addr list of
		 * association as Address Parameter of ASCONF CHUNK.
		 */
		bp = &asoc->base.bind_addr;
		p = bp->address_list.next;
		laddr = list_entry(p, struct sctp_sockaddr_entry, list);
		chunk = sctp_make_asconf_update_ip(asoc, &laddr->a, addrs,
						   addrcnt, SCTP_PARAM_ADD_IP);
		if (!chunk) {
			retval = -ENOMEM;
			goto out;
		}

		/* Add the new addresses to the bind address list with
		 * use_as_src set to 0.
		 */
		addr_buf = addrs;
		for (i = 0; i < addrcnt; i++) {
			addr = addr_buf;
			af = sctp_get_af_specific(addr->v4.sin_family);
			memcpy(&saveaddr, addr, af->sockaddr_len);
			retval = sctp_add_bind_addr(bp, &saveaddr,
						    sizeof(saveaddr),
						    SCTP_ADDR_NEW, GFP_ATOMIC);
			addr_buf += af->sockaddr_len;
		}
		if (asoc->src_out_of_asoc_ok) {
			struct sctp_transport *trans;

			list_for_each_entry(trans,
			    &asoc->peer.transport_addr_list, transports) {
				trans->cwnd = min(4*asoc->pathmtu, max_t(__u32,
				    2*asoc->pathmtu, 4380));
				trans->ssthresh = asoc->peer.i.a_rwnd;
				trans->rto = asoc->rto_initial;
				sctp_max_rto(asoc, trans);
				trans->rtt = trans->srtt = trans->rttvar = 0;
				/* Clear the source and route cache */
				sctp_transport_route(trans, NULL,
						     sctp_sk(asoc->base.sk));
			}
		}
		retval = sctp_send_asconf(asoc, chunk);
	}

out:
	return retval;
}